#if !defined(_SMBUSREGS_H_)
#define _SMBUSREGS_H_

#define REGINDEX_SMBUS_SDA			0
#define REGINDEX_SMBUS_STS			1

#define BITMASK_SMBUS_STS_SDAST		0x40
#define BITMASK_SMBUS_STS_BER		0x20
#define BITMASK_SMBUS_STS_NEGACK	0x10
#define BITMASK_SMBUS_STS_STASTR	0x08
#define BITMASK_SMBUS_STS_MASTER	0x02

#define	REGINDEX_SMBUS_CSTS			2

#define BITMASK_SMBUS_CSTS_BB		0x02

#define REGINDEX_SMBUS_CTRL1		3

#define BITMASK_SMBUS_CTRL1_STASTRE	0x80
#define BITMASK_SMBUS_CTRL1_NMINTE	0x40
#define BITMASK_SMBUS_CTRL1_ACK		0x10
#define BITMASK_SMBUS_CTRL1_STOP	0x02
#define BITMASK_SMBUS_CTRL1_START	0x01

#define REGINDEX_SMBUS_ADDR			4
#define REGINDEX_SMBUS_CTRL2		5

#define BITMASK_SMBUS_CTRL2_ENABLE	0x01

#define REGINDEX_SMBUS_CTRL3		6

#define READ_PORT_SDA(pDevExt)							\
	(READ_PORT_UCHAR ( (PUCHAR)							\
		pDevExt->ioBase + REGINDEX_SMBUS_SDA))
#define READ_PORT_STS(pDevExt)							\
	(READ_PORT_UCHAR ( (PUCHAR)							\
		pDevExt->ioBase + REGINDEX_SMBUS_STS))
#define READ_PORT_CSTS(pDevExt)						\
	(READ_PORT_UCHAR ( (PUCHAR)							\
		pDevExt->ioBase + REGINDEX_SMBUS_CSTS))
#define READ_PORT_CTRL1(pDevExt)						\
	(READ_PORT_UCHAR ( (PUCHAR)							\
		pDevExt->ioBase + REGINDEX_SMBUS_CTRL1))
#define READ_PORT_ADDR(pDevExt)						\
	(READ_PORT_UCHAR ( (PUCHAR)							\
		pDevExt->ioBase + REGINDEX_SMBUS_ADDR))
#define READ_PORT_CTRL2(pDevExt)						\
	(READ_PORT_UCHAR ( (PUCHAR)							\
		pDevExt->ioBase + REGINDEX_SMBUS_CTRL2))
#define READ_PORT_CTRL3(pDevExt)						\
	(READ_PORT_UCHAR ( (PUCHAR)							\
		pDevExt->ioBase + REGINDEX_SMBUS_CTRL3))

#define WRITE_PORT_SDA(pDevExt, bData)					\
	(WRITE_PORT_UCHAR ( (PUCHAR)						\
		pDevExt->ioBase + REGINDEX_SMBUS_SDA, bData))
#define WRITE_PORT_STS(pDevExt, bData)					\
	(WRITE_PORT_UCHAR ( (PUCHAR)						\
		pDevExt->ioBase + REGINDEX_SMBUS_STS, bData))
#define WRITE_PORT_CSTS(pDevExt, bData)				\
	(WRITE_PORT_UCHAR ( (PUCHAR)						\
		pDevExt->ioBase + REGINDEX_SMBUS_CSTS, bData))
#define WRITE_PORT_CTRL1(pDevExt, bData)				\
	(WRITE_PORT_UCHAR ( (PUCHAR)						\
		pDevExt->ioBase + REGINDEX_SMBUS_CTRL1, bData))
#define WRITE_PORT_ADDR(pDevExt, bData)				\
	(WRITE_PORT_UCHAR ( (PUCHAR)						\
		pDevExt->ioBase + REGINDEX_SMBUS_ADDR, bData))
#define WRITE_PORT_CTRL2(pDevExt, bData)				\
	(WRITE_PORT_UCHAR ( (PUCHAR)						\
		pDevExt->ioBase + REGINDEX_SMBUS_CTRL2, bData))
#define WRITE_PORT_CTRL3(pDevExt, bData)				\
	(WRITE_PORT_UCHAR ( (PUCHAR)						\
		pDevExt->ioBase + REGINDEX_SMBUS_CTRL3, bData))

#endif  // _SMBUSREGS_H_



