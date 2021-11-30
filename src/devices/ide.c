#include "devices/ide.h"
#include <ctype.h>
#include <debug.h>
#include <stdbool.h>
#include <stdio.h>
#include "devices/block.h"
#include "devices/partition.h"
#include "devices/timer.h"
#include "threads/io.h"
#include "threads/interrupt.h"
#include "threads/synch.h"

/* The code in this file is an interface to an ATA (IDE)
   controller.  It attempts to comply to [ATA-3]. */

/* ATA command block port addresses. */
#define reg_data(CHANNEL) ((CHANNEL)->reg_base + 0)     /* Data. */
#define reg_error(CHANNEL) ((CHANNEL)->reg_base + 1)    /* Error. */
#define reg_nsect(CHANNEL) ((CHANNEL)->reg_base + 2)    /* Sector Count. */
#define reg_lbal(CHANNEL) ((CHANNEL)->reg_base + 3)     /* LBA 0:7. */
#define reg_lbam(CHANNEL) ((CHANNEL)->reg_base + 4)     /* LBA 15:8. */
#define reg_lbah(CHANNEL) ((CHANNEL)->reg_base + 5)     /* LBA 23:16. */
#define reg_device(CHANNEL) ((CHANNEL)->reg_base + 6)   /* Device/LBA 27:24. */
#define reg_status(CHANNEL) ((CHANNEL)->reg_base + 7)   /* Status (r/o). */
#define reg_command(CHANNEL) reg_status (CHANNEL)       /* Command (w/o). */

/* ATA control block port addresses.
   (If we supported non-legacy ATA controllers this would not be
   flexible enough, but it's fine for what we do.) */
#define reg_ctl(CHANNEL) ((CHANNEL)->reg_base + 0x206)  /* Control (w/o). */
#define reg_alt_status(CHANNEL) reg_ctl (CHANNEL)       /* Alt Status (r/o). */

/* Alternate Status Register bits. */
#define STA_BSY 0x80            /* Busy. */
#define STA_DRDY 0x40           /* Device Ready. */
#define STA_DRQ 0x08            /* Data Request. */

/* Control Register bits. */
#define CTL_SRST 0x04           /* Software Reset. */

/* Device Register bits. */
#define DEV_MBS 0xa0            /* Must be set. */
#define DEV_LBA 0x40            /* Linear based addressing. */
#define DEV_DEV 0x10            /* Select device: 0=master, 1=slave. */

/* Commands.
   Many more are defined but this is the small subset that we
   use. */
#define CMD_IDENTIFY_DEVICE 0xec        /* IDENTIFY DEVICE. */
#define CMD_READ_SECTOR_RETRY 0x20      /* READ SECTOR with retries. */
#define CMD_WRITE_SECTOR_RETRY 0x30     /* WRITE SECTOR with retries. */

/* An ATA device. */
struct ata_disk
  {
    char name[8];               /* Name, e.g. "hda". */
    struct channel *channel;    /* Channel that disk is attached to. */
    int dev_no;                 /* Device 0 or 1 for master or slave. */
    bool is_ata;                /* Is device an ATA disk? */
  };

/* An ATA channel (aka controller).
   Each channel can control up to two disks. */
struct channel
  {
    char name[8];               /* Name, e.g. "ide0". */
    uint16_t reg_base;          /* Base I/O port. */
    uint8_t irq;                /* Interrupt in use. */

    struct lock lock;           /* Must acquire to access the controller. */
    bool expecting_interrupt;   /* True if an interrupt is expected, false if
                                   any interrupt would be spurious. */
    struct semaphore completion_wait;   /* Up'd by interrupt handler. */

    struct ata_disk devices[2];     /* The devices on this channel. */
  };

/* We support the two "legacy" ATA channels found in a standard PC. */
#define CHANNEL_CNT 2
static struct channel channels[CHANNEL_CNT];

static struct block_operations ide_operations;

static void reset_channel (struct channel *);
static bool check_device_type (struct ata_disk *);
static void identify_ata_device (struct ata_disk *);

static void select_sector (struct ata_disk *, block_sector_t);
static void issue_pio_command (struct channel *, uint8_t command);
static void input_sector (struct channel *, void *);
static void output_sector (struct channel *, const void *);

static void wait_until_idle (const struct ata_disk *);
static bool wait_while_busy (const struct ata_disk *);
static void select_device (const struct ata_disk *);
static void select_device_wait (const struct ata_disk *);

static void interrupt_handler (struct intr_frame *);

/* Initialize the disk subsystem and detect disks. */
void
ide_init (void) 
{
  size_t chan_no;

  for (chan_no = 0; chan_no < CHANNEL_CNT; chan_no++)
    {
      struct channel *c = &channels[chan_no];
      int dev_no;

      /* Initialize channel. */
      snprintf (c->name, sizeof c->name, "ide%zu", chan_no);
      switch (chan_no) 
        {
        case 0:
          c->reg_base = 0x1f0;
          c->irq = 14 + 0x20;
          break;
        case 1:
          c->reg_base = 0x170;
          c->irq = 15 + 0x20;
          break;
        default:
          NOT_REACHED ();
        }
      lock_init (&c->lock);
      c->expecting_interrupt = false;
      sema_init (&c->completion_wait, 0);
 
      /* Initialize devices. */
      for (dev_no = 0; dev_no < 2; dev_no++)
        {
          struct ata_disk *d = &c->devices[dev_no];
          snprintf (d->name, sizeof d->name,
                    "hd%c", 'a' + chan_no * 2 + dev_no); 
          d->channel = c;
          d->dev_no = dev_no;
          d->is_ata = false;
        }

      /* Register interrupt handler. */
      intr_register_ext (c->irq, interrupt_handler, c->name);

      /* Reset hardware. */
      reset_channel (c);

      /* Distinguish ATA hard disks from other devices. */
      if (check_device_type (&c->devices[0]))
        check_device_type (&c->devices[1]);

      /* Read hard disk identity information. */
      for (dev_no = 0; dev_no < 2; dev_no++)
        if (c->devices[dev_no].is_ata)
          identify_ata_device (&c->devices[dev_no]);
    }
}

/* Disk detection and identification. */

static char *descramble_ata_string (char *, int size);

/* Resets an ATA channel and waits for any devices present on it
   to finish the reset. */
static void
reset_channel (struct channel *c) 
{
  bool present[2];
  int dev_no;

  /* The ATA reset sequence depends on which devices are present,
     so we start by detecting device presence. */
  for (dev_no = 0; dev_no < 2; dev_no++)
    {
      struct ata_disk *d = &c->devices[dev_no];

      select_device (d);

      outb (reg_nsect (c), 0x55);
      outb (reg_lbal (c), 0xaa);

      outb (reg_nsect (c), 0xaa);
      outb (reg_lbal (c), 0x55);

      outb (reg_nsect (c), 0x55);
      outb (reg_lbal (c), 0xaa);

      present[dev_no] = (inb (reg_nsect (c)) == 0x55
                         && inb (reg_lbal (c)) == 0xaa);
    }

  /* Issue soft reset sequence, which selects device 0 as a side effect.
     Also enable interrupts. */
  outb (reg_ctl (c), 0);
  timer_usleep (10);
  outb (reg_ctl (c), CTL_SRST);
  timer_usleep (10);
  outb (reg_ctl (c), 0);

  timer_msleep (150);

  /* Wait for device 0 to clear BSY. */
  if (present[0]) 
    {
      select_device (&c->devices[0]);
      wait_while_busy (&c->devices[0]); 
    }

  /* Wait for device 1 to clear BSY. */
  if (present[1])
    {
      int i;

      select_device (&c->devices[1]);
      for (i = 0; i < 3000; i++) 
        {
          if (inb (reg_nsect (c)) == 1 && inb (reg_lbal (c)) == 1)
            break;
          timer_msleep (10);
        }
      wait_while_busy (&c->devices[1]);
    }
}

/* Checks whether device D is an ATA disk and sets D's is_ata
   member appropriately.  If D is device 0 (master), returns true
   if it's possible that a slave (device 1) exists on this
   channel.  If D is device 1 (slave), the return value is not
   meaningful. */
static bool
check_device_type (struct ata_disk *d) 
{
  struct channel *c = d->channel;
  uint8_t error, lbam, lbah, status;

  select_device (d);

  error = inb (reg_error (c));
  lbam = inb (reg_lbam (c));
  lbah = inb (reg_lbah (c));
  status = inb (reg_status (c));

  if ((error != 1 && (error != 0x81 || d->dev_no == 1))
      || (status & STA_DRDY) == 0
      || (status & STA_BSY) != 0)
    {
      d->is_ata = false;
      return error != 0x81;      
    }
  else 
    {
      d->is_ata = (lbam == 0 && lbah == 0) || (lbam == 0x3c && lbah == 0xc3);
      return true; 
    }
}

/* Sends an IDENTIFY DEVICE command to disk D and reads the
   response.  Registers the disk with the block device
   layer. */
static void
identify_ata_device (struct ata_disk *d) 
{
  struct channel *c = d->channel;
  char id[BLOCK_SECTOR_SIZE];
  block_sector_t capacity;
  char *model, *serial;
  char extra_info[128];
  struct block *block;

  ASSERT (d->is_ata);

  /* Send the IDENTIFY DEVICE command, wait for an interrupt
     indicating the device's response is ready, and read the data
     into our buffer. */
  select_device_wait (d);
  issue_pio_command (c, CMD_IDENTIFY_DEVICE);
  sema_down (&c->completion_wait);
  if (!wait_while_busy (d))
    {
      d->is_ata = false;
      return;
    }
  input_sector (c, id);

  /* Calculate capacity.
     Read model name and serial number. */
  capacity = *(uint32_t *) &id[60 * 2];
  model = descramble_ata_string (&id[10 * 2], 20);
  serial = descramble_ata_string (&id[27 * 2], 40);
  snprintf (extra_info, sizeof extra_info,
            "model \"%s\", serial \"%s\"", model, serial);

  /* Disable access to IDE disks over 1 GB, which are likely
     physical IDE disks rather than virtual ones.  If we don't
     allow access to those, we're less likely to scribble on
     someone's important data.  You can disable this check by
     hand if you really want to do so. */
  if (capacity >= 1024 * 1024 * 1024 / BLOCK_SECTOR_SIZE)
    {
      printf ("%s: ignoring ", d->name);
      print_human_readable_size (capacity * 512);
      printf ("disk for safety\n");
      d->is_ata = false;
      return;
    }

  /* Register. */
  block = block_register (d->name, BLOCK_RAW, extra_info, capacity,
                          &ide_operations, d);
  partition_scan (block);
}

/* Translates STRING, which consists of SIZE bytes in a funky
   format, into a null-terminated string in-place.  Drops
   trailing whitespace and null bytes.  Returns STRING.  */
static char *
descramble_ata_string (char *string, int size) 
{
  int i;

  /* Swap all pairs of bytes. */
  for (i = 0; i + 1 < size; i += 2)
    {
      char tmp = string[i];
      string[i] = string[i + 1];
      string[i + 1] = tmp;
    }

  /* Find the last non-white, non-null character. */
  for (size--; size > 0; size--)
    {
      int c = string[size - 1];
      if (c != '\0' && !isspace (c))
        break; 
    }
  string[size] = '\0';

  return string;
}

/* Reads sector SEC_NO from disk D into BUFFER, which must have
   room for BLOCK_SECTOR_SIZE bytes.
   Internally synchronizes accesses to disks, so external
   per-disk locking is unneeded. */
static void
ide_read (void *d_, block_sector_t sec_no, void *buffer)
{
  struct ata_disk *d = d_;
  struct channel *c = d->channel;
  lock_acquire (&c->lock);
  select_sector (d, sec_no);
  issue_pio_command (c, CMD_READ_SECTOR_RETRY);
  sema_down (&c->completion_wait);
  if (!wait_while_busy (d))
    PANIC ("%s: disk read failed, sector=%"PRDSNu, d->name, sec_no);
  input_sector (c, buffer);
  lock_release (&c->lock);
}

/* Write sector SEC_NO to disk D from BUFFER, which must contain
   BLOCK_SECTOR_SIZE bytes.  Returns after the disk has
   acknowledged receiving the data.
   Internally synchronizes accesses to disks, so external
   per-disk locking is unneeded. */
static void
ide_write (void *d_, block_sector_t sec_no, const void *buffer)
{
  struct ata_disk *d = d_;
  struct channel *c = d->channel;
  lock_acquire (&c->lock);
  select_sector (d, sec_no);
  issue_pio_command (c, CMD_WRITE_SECTOR_RETRY);
  if (!wait_while_busy (d))
    PANIC ("%s: disk write failed, sector=%"PRDSNu, d->name, sec_no);
  output_sector (c, buffer);
  sema_down (&c->completion_wait);
  lock_release (&c->lock);
}

static struct block_operations ide_operations =
  {
    ide_read,
    ide_write
  };

/* Selects device D, waiting for it to become ready, and then
   writes SEC_NO to the disk's sector selection registers.  (We
   use LBA mode.) */
static void
select_sector (struct ata_disk *d, block_sector_t sec_no)
{
  struct channel *c = d->channel;

  ASSERT (sec_no < (1UL << 28));
  
  select_device_wait (d);
  outb (reg_nsect (c), 1);
  outb (reg_lbal (c), sec_no);
  outb (reg_lbam (c), sec_no >> 8);
  outb (reg_lbah (c), (sec_no >> 16));
  outb (reg_device (c),
        DEV_MBS | DEV_LBA | (d->dev_no == 1 ? DEV_DEV : 0) | (sec_no >> 24));
}

/* Writes COMMAND to channel C and prepares for receiving a
   completion interrupt. */
static void
issue_pio_command (struct channel *c, uint8_t command) 
{
  /* Interrupts must be enabled or our semaphore will never be
     up'd by the completion handler. */
  ASSERT (intr_get_level () == INTR_ON);

  c->expecting_interrupt = true;
  outb (reg_command (c), command);
}

/* Reads a sector from channel C's data register in PIO mode into
   SECTOR, which must have room for BLOCK_SECTOR_SIZE bytes. */
static void
input_sector (struct channel *c, void *sector) 
{
  insw (reg_data (c), sector, BLOCK_SECTOR_SIZE / 2);
}

/* Writes SECTOR to channel C's data register in PIO mode.
   SECTOR must contain BLOCK_SECTOR_SIZE bytes. */
static void
output_sector (struct channel *c, const void *sector) 
{
  outsw (reg_data (c), sector, BLOCK_SECTOR_SIZE / 2);
}

/* Low-level ATA primitives. */

/* Wait up to 10 seconds for the controller to become idle, that
   is, for the BSY and DRQ bits to clear in the status register.

   As a side effect, reading the status register clears any
   pending interrupt. */
static void
wait_until_idle (const struct ata_disk *d) 
{
  int i;

  for (i = 0; i < 1000; i++) 
    {
      if ((inb (reg_status (d->channel)) & (STA_BSY | STA_DRQ)) == 0)
        return;
      timer_usleep (10);
    }

  printf ("%s: idle timeout\n", d->name);
}

/* Wait up to 30 seconds for disk D to clear BSY,
   and then return the status of the DRQ bit.
   The ATA standards say that a disk may take as long as that to
   complete its reset. */
static bool
wait_while_busy (const struct ata_disk *d) 
{
  struct channel *c = d->channel;
  int i;
  
  for (i = 0; i < 3000; i++)
    {
      if (i == 700)
        printf ("%s: busy, waiting...", d->name);
      if (!(inb (reg_alt_status (c)) & STA_BSY)) 
        {
          if (i >= 700)
            printf ("ok\n");
          return (inb (reg_alt_status (c)) & STA_DRQ) != 0;
        }
      timer_msleep (10);
    }

  printf ("failed\n");
  return false;
}

/* Program D's channel so that D is now the selected disk. */
static void
select_device (const struct ata_disk *d)
{
  struct channel *c = d->channel;
  uint8_t dev = DEV_MBS;
  if (d->dev_no == 1)
    dev |= DEV_DEV;
  outb (reg_device (c), dev);
  inb (reg_alt_status (c));
  timer_nsleep (400);
}

/* Select disk D in its channel, as select_device(), but wait for
   the channel to become idle before and after. */
static void
select_device_wait (const struct ata_disk *d) 
{
  wait_until_idle (d);
  select_device (d);
  wait_until_idle (d);
}

/* ATA interrupt handler. */
static void
interrupt_handler (struct intr_frame *f) 
{
  struct channel *c;

  for (c = channels; c < channels + CHANNEL_CNT; c++)
    if (f->vec_no == c->irq)
      {
        if (c->expecting_interrupt) 
          {
            inb (reg_status (c));               /* Acknowledge interrupt. */
            sema_up (&c->completion_wait);      /* Wake up waiter. */
          }
        else
          printf ("%s: unexpected interrupt\n", c->name);
        return;
      }

  NOT_REACHED ();
}


