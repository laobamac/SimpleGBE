/* Copyright 2024 王孝慈（laobamac）*/
/* MIT License */

#ifndef __APPLE_IGB_H__
#define __APPLE_IGB_H__

#define MBit 1000000

enum {
	eePowerStateOff = 0,
	eePowerStateOn,
	eePowerStateCount
};

enum
{
    MEDIUM_INDEX_AUTO = 0,
    MEDIUM_INDEX_10HD,
    MEDIUM_INDEX_10FD,
    MEDIUM_INDEX_100HD,
    MEDIUM_INDEX_100FD,
    MEDIUM_INDEX_100FDFC,
    MEDIUM_INDEX_1000FD,
    MEDIUM_INDEX_1000FDFC,
    MEDIUM_INDEX_1000FDEEE,
    MEDIUM_INDEX_1000FDFCEEE,
    MEDIUM_INDEX_100FDEEE,
    MEDIUM_INDEX_100FDFCEEE,
    MEDIUM_INDEX_COUNT
};


enum {
    kSpeed1000MBit = 1000*MBit,
    kSpeed100MBit = 100*MBit,
    kSpeed10MBit = 10*MBit,
};

enum {
    kFlowControlTypeNone = 0,
    kFlowControlTypeRx = 1,
    kFlowControlTypeTx = 2,
    kFlowControlTypeRxTx = 3,
    kFlowControlTypeCount
};

enum {
    kEEETypeNo = 0,
    kEEETypeYes = 1,
    kEEETypeCount
};

#define super IOEthernetController

class SimpleGBE: public super
{
	
	OSDeclareDefaultStructors(SimpleGBE);
	
public:
	// --------------------------------------------------
	// IOService (or its superclass) methods.
	// --------------------------------------------------
	
	virtual bool start(IOService * provider);
	virtual void stop(IOService * provider);
	virtual bool init(OSDictionary *properties);
	virtual void free();

	// --------------------------------------------------
	// Power Management Support
	// --------------------------------------------------
	virtual IOReturn registerWithPolicyMaker(IOService* policyMaker);
    virtual IOReturn setPowerState( unsigned long powerStateOrdinal, IOService *policyMaker );
    virtual void systemWillShutdown(IOOptionBits specifier);
	
	// --------------------------------------------------
	// IONetworkController methods.
	// --------------------------------------------------
	
	virtual IOReturn enable(IONetworkInterface * netif);
	virtual IOReturn disable(IONetworkInterface * netif);
	
	virtual UInt32 outputPacket(mbuf_t m, void * param);
	
	virtual void getPacketBufferConstraints(IOPacketBufferConstraints * constraints) const;
	
	virtual IOOutputQueue * createOutputQueue();
	
	virtual const OSString * newVendorString() const;
	virtual const OSString * newModelString() const;
	
	virtual IOReturn selectMedium(const IONetworkMedium * medium);
	virtual bool configureInterface(IONetworkInterface * interface);
	
	virtual bool createWorkLoop();
	virtual IOWorkLoop * getWorkLoop() const;
	
	//-----------------------------------------------------------------------
	// Methods inherited from IOEthernetController.
	//-----------------------------------------------------------------------
	
	virtual IOReturn getHardwareAddress(IOEthernetAddress * addr);
	virtual IOReturn setHardwareAddress(const IOEthernetAddress * addr);
	virtual IOReturn setPromiscuousMode(bool active);
	virtual IOReturn setMulticastMode(bool active);
	virtual IOReturn setMulticastList(IOEthernetAddress * addrs, UInt32 count);
	virtual IOReturn getChecksumSupport(UInt32 *checksumMask, UInt32 checksumFamily, bool isOutput);
	virtual IOReturn setMaxPacketSize (UInt32 maxSize);
	virtual IOReturn getMaxPacketSize (UInt32 *maxSize) const;
	virtual IOReturn getMinPacketSize (UInt32 *minSize) const;
    virtual IOReturn setWakeOnMagicPacket(bool active);
    virtual IOReturn getPacketFilters(const OSSymbol * group, UInt32 * filters) const;
    virtual UInt32 getFeatures() const;

private:
	IOWorkLoop* workLoop;
	IOPCIDevice* pdev;
	OSDictionary * mediumDict;
	IONetworkMedium * mediumTable[MEDIUM_INDEX_COUNT];
	IOOutputQueue * transmitQueue;
	
	IOInterruptEventSource * interruptSource;
	IOTimerEventSource * watchdogSource;
	IOTimerEventSource * resetSource;
	IOTimerEventSource * dmaErrSource;

	IOEthernetInterface * netif;
	IONetworkStats * netStats;
	IOEthernetStats * etherStats;

	IOMemoryMap * csrPCIAddress;
	
	IOMbufNaturalMemoryCursor * txMbufCursor;

	bool enabledForNetif;
	bool bSuspended;
	bool useTSO;

    bool linkUp;
    bool stalled;

    UInt16 eeeMode;

	UInt32 iff_flags;
	UInt32 _features;
	UInt32 preLinkStatus;
	UInt32 powerState;
	UInt32 _mtu;
    SInt32 txNumFreeDesc;

    UInt32 chip_idx;

	struct igb_adapter priv_adapter;
public:
	void startTxQueue();
	void stopTxQueue();
	UInt32 mtu() { return _mtu; }
	UInt32 flags(){ return iff_flags;}
	UInt32 features() { return _features; }
	igb_adapter* adapter(){ return &priv_adapter; }
	IONetworkStats* getNetStats(){ return netStats; }
	IOEthernetStats* getEtherStats() { return etherStats; }
	dma_addr_t mapSingle( mbuf_t );
	void receive(mbuf_t skb );
	void setVid(mbuf_t skb, UInt16 vid);
	IOMbufNaturalMemoryCursor * txCursor(){ return txMbufCursor; }
	void rxChecksumOK( mbuf_t, UInt32 flag );
	bool running(){return enabledForNetif;}
	bool queueStopped(){return txMbufCursor == NULL || stalled;}
	bool carrier();
	void setCarrier(bool);
    
    void setTimers(bool enable);
private:
	void interruptOccurred(IOInterruptEventSource * src, int count);
	
	void watchdogTask();
	void updatePhyInfoTask();

    void intelRestart();
    bool intelCheckLink(struct igb_adapter *adapter);
    void setLinkUp();
    void setLinkDown();
    void checkLinkStatus();


    UInt16 intelSupportsEEE(struct igb_adapter *adapter);
    bool setupMediumDict();

    void intelSetupAdvForMedium(const IONetworkMedium *medium);
	bool addNetworkMedium(UInt32 type, UInt32 bps, UInt32 index);

	bool initEventSources( IOService* provider );

	bool igb_probe();
	void igb_remove();

	bool getBoolOption(const char *name, bool defVal);
	int getIntOption(const char *name, int defVal, int maxVal, int minVal );

public:
	static void interruptHandler(OSObject * target,
								 IOInterruptEventSource * src,
								 int count );
	
	

	static void watchdogHandler(OSObject * target, IOTimerEventSource * src);
	static void resetHandler(OSObject * target, IOTimerEventSource * src);

};


#endif //__APPLE_IGB_H__
