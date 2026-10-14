//
// File : UsbManagerLinux.cpp
//
// Implements the linux UsbManager class
//

#include "PAL_platformheader.h"

#include <libusb-1.0/libusb.h>

#include <sys/mount.h>

#include "RealTimeController.h"
#include "UsbManager.h"
#include "UsbManagerLinux.h"
#include "IUsbCodeUpdater.h"
#include "AtmelSambaUpdater.h"
#include "HobVersion.h"

#include "SessionEvent.h"

#include "WeigherSystemReset.h"
#include "PeripheralCodeUpdate.h"
#include "UsbAddComponent.h"

#include "AccessKeyActivation.h"
#include "AccessKeyDeactivation.h"
#include "UsbKbdAttachChange.h"

#include "Configuration.h"
#include "SessionMessage.h"
#include "AccessKeys.h"
#include "filesys/directory.h"

#include "Debug.h"
#include "Hex.h"
#include "Integer.h"
#include "UInteger.h"
#include "HobString.h"
#include "OutputLogger.h"
#include "FCError.h"
#include "SimpleTimer.h"
#include "device_event.h"

#include "WghIoctl.h" //from weigher driver
#include "linux/usb_linux_node.h"
#include "AtmelDeviceHandler.h"

#define USB_TRIGGER_MULTIPLE_TEST_LABELS
#ifdef USB_TRIGGER_MULTIPLE_TEST_LABELS
#include "PrinterPrintTestLabel.h"
#endif


UsbManagerLinux::UsbManagerLinux()
{
    Debug::trace("UsbManagerLinux.Ctor", HobString("Constructor"));

    addrpref = 0;

    foundMouse = false;
    foundKeyboard = false;
    foundBarcodeScanner = false;
	foundStorage = false;

    powerUp = true;
    addSamba = false;
	Debug::trace("UsbManagerLinux.Ctor", HobString("Constructor: addSamba=FALSE"));

    just_processed = 0;

	libusb_init(NULL);

    // to enable libusb debug output
    libusb_set_debug(NULL, 3);

	nmList.clear();
	
	//discovery();	//done in monitor USB
}

UsbManagerLinux::~UsbManagerLinux()
{
    Debug::trace("UsbManagerLinux.Dtor", HobString("Destructor"));

    libusb_exit(NULL);
}

///////////////////////////////////////////////////////////////////////////////
//Parse and show the different USB device descriptors entries.  Expects a
//valid point to the device descriptor.
//
//@param[in] desc- Pointer to the device descriptor
//
void UsbManagerLinux::UsbShowDevDesc(struct usb_device_descriptor *desc)
{
    if (NULL == desc)
    {
    	Debug::trace("UsbManagerLinux.Attach", HobString("UsbShowDevDesc():  desc is NULL. Exiting."));
	    return;
	}

#if 1
    if (Debug::isTracing("UsbManagerLinux.Attach"))
	{
		Debug::trace("UsbManagerLinux.Attach", HobString("****  UsbShowDevDesc:"));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  Length: ") + Integer::toString(desc->bLength));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  DescriptorType: ") + Hex::toString(desc->bDescriptorType));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  USB version: ") + Hex::toString(desc->bcdUSB));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  Vendor:Product: ") + Hex::toString(desc->idVendor) + ":" + Hex::toString(desc->idProduct));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  MaxPacketSize0: ") + Integer::toString(desc->bMaxPacketSize0));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  NumConfigurations: ") + Integer::toString(desc->bNumConfigurations));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  Device version: ") + Hex::toString(desc->bcdDevice));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  Device Class: ") + Hex::toString(desc->bDeviceClass));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  SubClass: ") + Hex::toString(desc->bDeviceSubClass));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  Protocol: ") + Hex::toString(desc->bDeviceProtocol));
	}
#else
    PAL_absclass::PAL_OutputDbgStr("  Length              = %d%s", desc->bLength,
            desc->bLength == USB_DT_DEVICE_SIZE ? "" : " (!!!)");
    PAL_absclass::PAL_OutputDbgStr("  DescriptorType      = %02x", desc->bDescriptorType);
    PAL_absclass::PAL_OutputDbgStr("  USB version         = %04x", desc->bcdUSB);
    PAL_absclass::PAL_OutputDbgStr("  Vendor:Product      = %x:%x",
            desc->idVendor, desc->idProduct);
    PAL_absclass::PAL_OutputDbgStr("  MaxPacketSize0      = %d", desc->bMaxPacketSize0);
    PAL_absclass::PAL_OutputDbgStr("  NumConfigurations   = %d", desc->bNumConfigurations);
    PAL_absclass::PAL_OutputDbgStr("  Device version      = %04x", desc->bcdDevice);
    PAL_absclass::PAL_OutputDbgStr("  Device Class:SubClass:Protocol = %02x:%02x:%02x",
            desc->bDeviceClass, desc->bDeviceSubClass, desc->bDeviceProtocol);
#endif
    switch (desc->bDeviceClass) {
        case 0:
            Debug::trace("UsbManagerLinux.Attach", "    Per-interface classes");
            break;
        case 9:
            Debug::trace("UsbManagerLinux.Attach", "    Hub device class");
            break;
        case 0xff:
            Debug::trace("UsbManagerLinux.Attach", "    Vendor class");
            break;
        default:
            Debug::trace("UsbManagerLinux.Attach", "    Unknown class");
    }
}

///////////////////////////////////////////////////////////////////////////////
//Parse and show the different USB Config descriptors entries.  Expects a
//valid point to the device descriptor.
//
//@param[in] dev_config- Pointer to the configuration descriptor
//
void UsbManagerLinux::UsbShowConfigDesc(struct usb_config_descriptor *dev_config)
{
    if (NULL == dev_config)
    {
    	Debug::trace("UsbManagerLinux.Attach", HobString("UsbShowConfigDesc():  dev_config is NULL. Exiting."));
	    return;
	}
#if 1
    if (Debug::isTracing("UsbManagerLinux.Attach"))
	{
	    Debug::trace("UsbManagerLinux.Attach", HobString("****  UsbShowConfigDesc:"));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  bNumInterfaces: ") + Integer::toString(dev_config->bNumInterfaces));
	}
#else
    PAL_absclass::PAL_OutputDbgStr("Printing Config descriptor");
    PAL_absclass::PAL_OutputDbgStr("bNumInterfaces %d", dev_config->bNumInterfaces);
#endif
}
///////////////////////////////////////////////////////////////////////////////
//Parse and show the different USB interface descriptors entries.  Expects a
//valid point to the device descriptor.
//
//@param[in] dev_iface- Pointer to the interface descriptor
//
void UsbManagerLinux::UsbShowIfaceDesc(struct usb_interface_descriptor *dev_iface)
{
    if (NULL == dev_iface)
    {
    	Debug::trace("UsbManagerLinux.Attach", HobString("UsbShowIfaceDesc():  dev_iface is NULL. Exiting."));
	    return;
	}
#if 1
    if (Debug::isTracing("UsbManagerLinux.Attach"))
	{
	    Debug::trace("UsbManagerLinux.Attach", HobString("****  UsbShowIfaceDesc:"));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  bNumEndpoints: ") + Integer::toString(dev_iface->bNumEndpoints));
	}
#else
    PAL_absclass::PAL_OutputDbgStr("Printing Interface descriptor");
    PAL_absclass::PAL_OutputDbgStr("bNumEndpoints %d", dev_iface->bNumEndpoints);
#endif
}
///////////////////////////////////////////////////////////////////////////////
//Parse and show the different USB Endpoint descriptors entries.  Expects a
//valid point to the device descriptor.
//
//@param[in] dev_ep- Pointer to the endpoint descriptor
//

void UsbManagerLinux::UsbShowEpDesc(struct usb_endpoint_descriptor *dev_ep)
{
    if (NULL == dev_ep)
    {
    	Debug::trace("UsbManagerLinux.Attach", HobString("UsbShowEpDesc():  dev_ep is NULL. Exiting."));
	    return;
	}
#if 1
    if (Debug::isTracing("UsbManagerLinux.Attach"))
	{
	    Debug::trace("UsbManagerLinux.Attach", HobString("****  UsbShowEpDesc:"));
	    Debug::trace("UsbManagerLinux.Attach", HobString("  bEndpointAddress: ") + Integer::toString(dev_ep->bEndpointAddress));
	}
#else
    PAL_absclass::PAL_OutputDbgStr("Printing EP descriptor");
    PAL_absclass::PAL_OutputDbgStr("bNumEndpoints %d", dev_ep->bEndpointAddress);
#endif
}

//
// HandleDeviceNotification
//
// Purpose:
//
// Handle device notification events.
//
// Parameters:
//
//  PEVENT_INFO event_info
//              Pointer to structure which holds dev_path and event_type flag.
//
//
// Description:
//
// The daemon registers for notifications at startup. All notifications of
// device arrival and removal are then passed on to us to handle.
//
// When the USB device is plugged in (attachment event) a unique node id is created.
// The device is then determined if it is a managend device or non managed device.
// Managed devices are add to the device map if not already present and Non
// managed devices (Non Hobart) are kept track of but not added to the device map. 
// RTC is notified to create a service class instance for the device.
//
// When the USB device is unplugged (detachment event) the unique id is 
// used to search the device map and removed if found. If the device was not found then
// the non managed list of devices is searched and if device found removed. 
// The RTC is notified to delete the service class instance for the device if managed.
//
void UsbManagerLinux::HandleDeviceNotification(PEVENT_INFO event_info, int32_t eVid, int32_t ePid)
{
	//SOF-4076 
	//allow libusb to enumerate the device before we start otherwise the device will be missing from the libusb device list.
	PAL_absclass::PAL_sleep(500);	
	Debug::trace("UsbManagerLinux.Event", "HandleDeviceNotification() -- pre-lock" );
	if( NULL != event_info ) {
		UsbCritSec.Enter();	

		Debug::trace("UsbManagerLinux.Event", "HandleDeviceNotification() -- lock" );

		string fullpath = event_info->dev_path;

		USBD_NODE_ID nodeId = NodeIdFromDeviceName(fullpath.c_str());
		// device bus and address must be retrieved to properly handle multiple devices 
		// with the same vid, pid.
		uint8_t bus = getBusFromDeviceName( fullpath.c_str() );
		uint8_t address = getAddressFromDeviceName( fullpath.c_str() );

		//determine what to do to the device.
		if( HOB_DEVICE_ATTACHED == event_info->event_type ) {
			Debug::trace("UsbManagerLinux.Event", "Handle device attach event for: " + fullpath + " vid: " + 
			              Hex::toString(eVid) + " pid: " + Hex::toString(ePid) );
			//has this device already been added to the map?
			if( nodeId ) {				
				CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );
				if( iter == usbNodeMap.end() ) {
					CUsbNode *pNode = findMyDevice( eVid, ePid, nodeId, bus, address ); 
					if( pNode != NULL ) {
						//only notify RTC to attach the device if it is managed.
						if( isManagedDevice( pNode->getVendorId(), pNode->getProductId() ) ) {
							if( isDeviceAtmelSamba( eVid, ePid ) ) {				
									//AtmelDeviceHandler::getInstance()->ProcessDevice( nodeId, pNode->getBusNumber(), pNode->getDeviceAddress() );
									AtmelDeviceHandler::getInstance()->DeviceListArrival();
							} else {	
								Debug::trace("UsbManagerLinux.Event", " AttachDevice nodeId: " + Hex::toString(nodeId) + 
											 " bus: " + Integer::toString(pNode->getBusNumber()) + " address: " + Integer::toString(pNode->getDeviceAddress()));						
								if ( AttachDevice( nodeId ) ) {
									addToMap( nodeId, pNode );
									Debug::trace("UsbManagerLinux.Event", "Device added to map! Vid: " + Hex::toString( pNode->getVendorId() ) +
									" Pid: " + Hex::toString( pNode->getProductId() ) + " NodeId: " + Hex::toString( nodeId ) );
								} else {
									Debug::trace("UsbManagerLinux.Error", "Failed to attach device! Vid: " + Hex::toString( pNode->getVendorId() ) +
									" Pid: " + Hex::toString( pNode->getProductId() ) + " NodeId: " + Hex::toString( nodeId ) );
									delete pNode;
									pNode = NULL;
								}
							}
						} 
					}
				} else {
					Debug::trace("UsbManagerLinux.Event","Device already in map: " + fullpath );	
				}
			} else {
				Debug::trace("UsbManagerLinux.Error","Failed to create node for device: " + fullpath );
			}
		} else if( HOB_DEVICE_DETACHED == event_info->event_type ) {
			Debug::trace("UsbManagerLinux.Event", "Handle device detachment event for: " + fullpath + " Vid: " + 
			 			  Hex::toString(eVid) + " Pid: " + Hex::toString(ePid) );
			//only remove if valid nodeid, vid, pid 
			if( ( nodeId ) && ( !eVid ) && ( !ePid ) ) {				
        		CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );
				if( iter != usbNodeMap.end() ) {
					removeFromMap( nodeId );
					Debug::trace("UsbManagerLinux.Event","Removed node from map: " + Hex::toString(nodeId));
				} else {
					//Handle non managed devices
					if( !nmList.empty() ) {						
						updateNMList( nodeId );
					}
				}
			}
		} else {
			Debug::trace("UsbManagerLinux.Error","Unknown USB event type: " + Integer::toString(event_info->event_type));
		}
		UsbCritSec.Leave();
	}
}


// This function will try and find the device in the libusb device list and claim it
// if it is a managed (Hobart) device. If the device is not managed then it is checked 
// if the device id of HID type. Mouse, Keyboard and Barcode scanner need to be tracked 
// for the application notification but do not need to be managed the same way the 
// Hobart devices are being managed.    
CUsbLinuxNode *UsbManagerLinux::findMyDevice( int32_t vid, int32_t pid, USBD_NODE_ID nodeId, uint8_t bus, uint8_t address )
        {

	CUsbLinuxNode *pResult = NULL;
	libusb_device **list;
    ssize_t cnt = libusb_get_device_list( NULL, &list );
	Debug::trace("UsbManagerLinux.Event","findMyDevice()");
    if( 0 <= cnt ) {
        // iterate the list
		for ( ssize_t i = 0; i < cnt; i++ )
            {
			libusb_device *device = list[i];
			libusb_device_descriptor desc;

			if (libusb_get_device_descriptor(device, &desc) == 0) {
				bool match = false;

				//Debug::trace("UsbManagerLinux.Event", "Match device: " +  + 
				//				" bus: " + Integer::toString(bus) + 
				//				" dbus: " + Integer::toString(libusb_get_bus_number(device) ) +
				//				" address: " + Integer::toString(address) +
				//				" daddress: " + Integer::toString(libusb_get_device_address(device)) );		

				//used to handle multiple devices of the same type
				if( ( bus != 0 ) && ( address != 0 ) ) {
					if( ( libusb_get_bus_number(device) == bus ) && 
					    ( libusb_get_device_address(device) == address ) ) {
								 
						match = true;
					}							
				} else {
					//ignore bus and address for this device
					match = true; 
				}

				//is this my device?
				if( ( desc.idVendor == vid ) && ( desc.idProduct == pid ) && match ) {
					//create a node for our device
					CUsbLinuxNode *pNode = new CUsbLinuxNode;
					Debug::trace("UsbManagerLinux.Event", "Device found pNode: " + Hex::toString( ( long long ) pNode ) + " created" );
					if( pNode ) {
						//is this a managed Hobart device?
						if( isManagedDevice( vid, pid ) ) {
							Debug::trace("UsbManagerLinux.Event", "Managed device found in list: " + Hex::toString( vid ) + " " + Hex::toString( pid ) +
							" bus: " + Integer::toString(bus) + " address: " + Integer::toString(address) );
							pResult = pNode;
							Debug::trace("UsbManagerLinux.Event", "pNode->Initialize nodeId: " + Hex::toString(nodeId) + " device: " + Hex::toString((long long) device ) );

							pNode->Initialize((void *)device);
							addToMap( nodeId, pNode );

							if( isCodeUpdateRequired( vid, pid ) ) {
								if( isDeviceAtmelSamba( vid, pid ) ) {	
									//add to AtmelHandler device list			
									AtmelDeviceHandler::getInstance()->ProcessDevice( nodeId, pNode->getBusNumber(), pNode->getDeviceAddress() );
								}
							} 
						} else {
							//Non Hobart managed (HID) devices that still need to be tracked but not claimed.
							Debug::trace("UsbManagerLinux.Event", "Non Managed device found in list: " + Hex::toString( vid ) + " " + Hex::toString( pid ) );
							
							libusb_config_descriptor *pDevConfig = new libusb_config_descriptor[desc.bNumConfigurations];
							if( pDevConfig != NULL ) {
								//intialize the device config descriptor
								for( int i = 0; i < desc.bNumConfigurations; ++i ) {									
									libusb_get_config_descriptor( device, i, &pDevConfig );
								}

								//potential future use..
								if( pDevConfig->interface[0].altsetting[0].bInterfaceClass == 8 ) {
									Debug::trace("UsbManagerLinux.Event", HobString("Storage device attached to system: ") + Hex::toString((long)nodeId));	
											//this device is tracked by a python script. 
											NMTrack nm;
											nm.device = NM_HID_STORAGE;
											nm.id = nodeId;
											nmList.push_back(nm);

											HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::STORAGE_DEVICE_ATTACHED);
											dispatchEvent(&deviceEvent);
											foundStorage = true;
								}
								if( pDevConfig->interface[0].altsetting[0].bInterfaceClass == 3 ) { //HID
									NMTrack nm;
									//ignore our internal touchscreen device
									if( desc.idVendor != TOUCHSCREEN_PRODUCT_ID ) {
										//what type of HID device do we have?
										if( pDevConfig->interface[0].altsetting[0].bInterfaceProtocol == 2 ) {
											Debug::trace("UsbManagerLinux.Event", HobString("Mouse attached to system: ") + Hex::toString((long)nodeId));
											//pNode->state = CUsbNode::HID_MOUSE;
											HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::MOUSE_ATTACHED);
											//notify the session of the event
											dispatchEvent(&deviceEvent);

											//update our non managed device list
											nm.device = NM_HID_MOUSE;
											nm.id = nodeId;
											nmList.push_back(nm);

											foundMouse = true;
										} else {
											if( pDevConfig->interface[0].altsetting[0].iInterface != 5 ) {
												Debug::trace("UsbManagerLinux.Event", "Keyboard attached to system: " + Hex::toString((long)nodeId));
												//pNode->state = CUsbNode::HID_KEYBOARD;
												HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::KEYBOARD_ATTACHED);
												//notify the session of the event
												dispatchEvent(&deviceEvent);

												//update our non managed device list
												nm.device = NM_HID_KEYBOARD;
												nm.id = nodeId;
												nmList.push_back(nm);

												foundKeyboard = true;
											} else {
												Debug::trace("UsbManagerLinux.Event", "Barcode Scanner attached to system: " + Hex::toString((long)nodeId));
												pNode->state = CUsbNode::HID_BARCODE;
												HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::BARCODE_SCANNER_ATTACHED);
												//notify the session of the event
												dispatchEvent(&deviceEvent);

												//update our non managed device list
												nm.device = NM_HID_BARSCANNER;
												nm.id = nodeId;
												nmList.push_back(nm);

												foundBarcodeScanner = true;
											}
										}
									}
								} 
								//We do not claim non managed devices 
								delete pNode;
								delete pDevConfig;
							} else {
								Debug::trace("UsbManagerLinux.Error", "Failed to create new device configuration!" );
							}
						} 
					} else {
						Debug::trace("UsbManagerLinux.Error", "Failed to create new CUsbNode!" );
					}
					break;
				}
			} else {
				Debug::trace("UsbManagerLinux.Error", "Failed to get device descriptor!");
			}
		}
	} else {
		Debug::trace("UsbManagerLinux.Error", "Failed to get device list!");	
	}
	Debug::trace("UsbManagerLinux.Event", "findMyDevice() -- Exit");		
	return pResult;
            }


// The non managed HID type devices are removed from the non managed list of devices
// and the application is notified when the device is either attached or detached.
// This method is called when the device is removed. We find the node id of the missing 
// device and remove it from the list of non managed devices and the application is notified 
// of the device event.
void UsbManagerLinux::updateNMList( USBD_NODE_ID id )
{
	std::vector<NMTrack>::iterator iter = nmList.begin();
	Debug::trace("UsbManagerLinux.Event","updateNMList()");
	Debug::trace("UsbManagerLinux.Event","iter->id: " + Hex::toString(iter->id) );
	Debug::trace("UsbManagerLinux.Event","iter->device: " + Hex::toString(iter->device) );
	while( iter != nmList.end() ){
		if( iter->id == id ) {
			switch( iter->device ) 
            {
				case NM_HID_UNKOWN: {
					break;
				}
				case NM_HID_STORAGE: {
					//python script tracks storage devices
					Debug::trace("UsbManagerLinux.Event","Non managed storage device removed from system.");
					HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::STORAGE_DEVICE_REMOVED);
					dispatchEvent(&deviceEvent);
					nmList.erase(iter);
					
					foundStorage = false;
					break;
				}
				case NM_HID_MOUSE: {
					Debug::trace("UsbManagerLinux.Event","Non managed mouse device removed from system.");
					HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::MOUSE_REMOVED);
					dispatchEvent(&deviceEvent);
					
					nmList.erase(iter);
					
					foundMouse = false;
					break;
				}
				case NM_HID_KEYBOARD: {
					Debug::trace("UsbManagerLinux.Event","Non managed keyboard device removed from system.");
					HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::KEYBOARD_REMOVED);
					dispatchEvent(&deviceEvent);
					
					nmList.erase(iter);
					
					foundKeyboard = false;
					break;
                }
				case NM_HID_BARSCANNER: {
					Debug::trace("UsbManagerLinux.Event","Non manager barcode scanner device removed from system.");
					HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::BARCODE_SCANNER_REMOVED);
					dispatchEvent(&deviceEvent);
					
					nmList.erase(iter);

					foundBarcodeScanner = false;
					break;
				}
			}
		}
		if( ( !nmList.empty() ) && ( iter != nmList.end() ) ) {
			iter++;				
		}			
	}
            }

// This method should only be used at startup. It is used to capture all devices
// attached to the system at powerup. Device events are not received for device present 
// when the scale powers up they are missed due to chobart_d starting late. 
// Get a list of devices already attached and update the device map or device list 
// if it is a device of interest. Once the scale service is up we no longer need to 
// auto discover devices attached or detached from the system.
void UsbManagerLinux::discovery()
        {
	libusb_device **list;
	
	UsbCritSec.Enter();	
    
	ssize_t cnt = libusb_get_device_list( NULL, &list );
	char id[64];
	unsigned int mDeviceCnt = 0;
	
	Debug::trace("UsbManagerLinux.Ctor", "discovery()");
	Debug::trace("UsbManagerLinux.Ctor", "List size: " + Integer::toString( cnt ) );
    if( 0 <= cnt ) {
        // iterate the list
		for ( ssize_t i = 0; i < cnt; i++ )
            {
			Debug::trace("UsbManagerLinux.Ctor", "i: " + Integer::toString( i ) );
			libusb_device *device = list[i];
			Debug::trace("UsbManagerLinux.Ctor", "device from list: " + Hex::toString((long long) device));
			libusb_device_descriptor desc;
			if( libusb_get_device_descriptor(device, &desc) == 0 ) {
				//create node id for device
				memset( (void *)&id[0], 0, size_t(sizeof(id)) );
				sprintf(id, "%s%03d/%03d", 
							"/dev/bus/usb/",
							libusb_get_bus_number(device),
							libusb_get_device_address(device));

				
				USBD_NODE_ID nodeId = NodeIdFromDeviceName( (const char *)&id[0] );
				Debug::trace("UsbManagerLinux.Ctor", "Device to find: " + Hex::toString((long long) device ) + " vid: " + Hex::toString( desc.idVendor ) + " pid: " + Hex::toString( desc.idProduct ));
				CUsbNode *pNode = findMyDevice( desc.idVendor, desc.idProduct, nodeId, libusb_get_bus_number(device), libusb_get_device_address(device) ); 
				Debug::trace("UsbManagerLinux.Ctor", "pNode: " + Hex::toString( (long long)pNode) );
				if( pNode != NULL ) {						
					if( isManagedDevice( pNode->getVendorId(), pNode->getProductId() ) ) {							
							if( !isDeviceAtmelSamba( pNode->getVendorId(), pNode->getProductId() ) ) {
								addToMap( nodeId, pNode );
								Debug::trace("UsbManagerLinux.Ctor", "Attaching managed device: " + Integer::toString(nodeId) );
								if ( !AttachDevice( nodeId ) ) {
									Debug::trace("UsbManagerLinux.Ctor", "Failed to attach managed device");
								}
							} else {
                	addSamba = true;
                }
            }
					mDeviceCnt++;
				} 
            }
        }
		Debug::trace("UsbManagerLinux.Ctor", "Discovery found " + Integer::toString( mDeviceCnt ) + " managed devices and " + Integer::toString( nmList.size() ) + " non manged devices");
		if( addSamba ) {
			if( AtmelDeviceHandler::getInstance()->isIdle() ) {
				Debug::trace("UsbManagerLinux.Ctor", "Set the Atmel handler in motion");
				//have the handler process all atmel devices on the bus
				AtmelDeviceHandler::getInstance()->DeviceListArrival();
			} else {
				Debug::trace("UsbManagerLinux.Ctor", "Atmel handler is busy!");
				//TO DO: handle differently than sleep.....
				PAL_absclass::PAL_sleep(200);
				AtmelDeviceHandler::getInstance()->DeviceListArrival();
			}
			addSamba = false;
    }
	} else {
		Debug::trace("UsbManagerLinux.Ctor", "Libusb device list is empty!"); 
    }
    UsbCritSec.Leave();
	Debug::trace("UsbManagerLinux.Ctor", "discovery() -- exit");
}

// Parse the devName string to derive the required substrings.
// Concatenate the substrings to form a device instance ID string.
// Compute and return the unique node value based on the instance ID.
USBD_NODE_ID UsbManagerLinux::NodeIdFromDeviceName(const char * devName)
{
    USBD_NODE_ID nodeId = 0;

    // convert to ascii and force upper case
    std::string s(devName);
#ifdef CONVERT_TO_UPPERCASE
    std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::toupper);
#endif      // CONVERT_TO_UPPERCASE

    // hash device name into node value.
    CUsbNode *usb_node = new CUsbLinuxNode;
    if (NULL != usb_node)
    {
        nodeId = usb_node->createNodeId((char*) s.c_str());
        delete usb_node;
    }

    Debug::trace("UsbManagerLinux.Attach", HobString("NodeIdFromDeviceName: '") + s + string("' -> node ") + Hex::toString((ULONG)nodeId));
    return nodeId;
}

unsigned char UsbManagerLinux::getBusFromDeviceName( const char *pDevName )
{
	//bus number and address are always in same fixed location in the device name string
	unsigned char bus = 0;
	pDevName += 14;
	bus = atoi( (const char *)pDevName++ );
	Debug::trace("UsbManagerLinux.Attach", "getBusFromDeviceName bus: " + Integer::toString(bus) );
	return bus;
}

unsigned char UsbManagerLinux::getAddressFromDeviceName( const char *pDevName )
{
	//bus number and address are always in same fixed location in the device name string
	unsigned char address = 0;
	pDevName += 18;
	address = atoi( (const char *)pDevName++ );
	Debug::trace("UsbManagerLinux.Attach", "getBusFromDeviceName address: " + Integer::toString(address) );
	return address;
}


SUsbDescriptors * UsbManagerLinux::getUsbDescs( USBD_NODE_ID  nodeId )
{
    SUsbDescriptors * pd = NULL;

    UsbCritSec.Enter();

    Debug::trace("UsbManagerLinux.Attach", string("getUsbDescs(") + Hex::toString((ULONG)nodeId) + string(")"));

    CUsbNodeMap::const_iterator iter =  usbNodeMap.find( nodeId );

    if ( iter != usbNodeMap.end() )
    {
        CUsbNode* p = iter->second;
		if (p)
		{
	        pd = &(p->getDescriptors());
		    Debug::trace("UsbManagerLinux.Attach", string("getUsbDescs(") + Hex::toString((ULONG)nodeId) + string("):   Show CUsbNode:"));
			p->show();
	    }
    }

    UsbCritSec.Leave();

    return pd;
}


#if 0
// UpdateDeviceMap identifies the USB devices currently attached to the
// system,and maintains the usbNodeMap in agreement. It creates and
// initializes a CUsbNode for each detected device. If the node is not
// already in the usbNodeMap, it adds the CUsbNode to the map.
//
// After the map has been updated, it walks the map and for managed
// devices, it launches the code updater for devices requiring an initial
// download, otherwise it notifies RTC to start the service class for the
// device. Lastly, it removes any nodes which were already in the map but
// were not detected, terminating the updater or notifying the RTC if needed.
void UsbManagerLinux::UpdateDeviceMap()
{
    CUsbNode *pUsbNode = 0;

    Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: "));

    libusb_device **list;

    ssize_t cnt = libusb_get_device_list( NULL, &list );

    if ( 0 <= cnt )
    {
    	UsbCritSec.Enter();

        // clear detected flag on all nodes
        for ( CUsbNodeMap::iterator i( usbNodeMap.begin() ); i != usbNodeMap.end(); ++i )
            if ( pUsbNode = i->second )
            	pUsbNode->bDetected = false;

        // iterate the list
		for ( ssize_t i = 0; i < cnt; i++ )
		{
			libusb_device *device = list[i];
			libusb_device_descriptor desc;

			if (libusb_get_device_descriptor(device, &desc) == 0)
			{

				//do not create any hub class nodes
				if (desc.bDeviceClass != 9)
				{
					//
					// Create the CUsbNode.
					//
					pUsbNode = new CUsbLinuxNode();

					if (!pUsbNode)
					{
						//
						// failed to create the node, skip to next device
						//
						Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: failed to create node"));
						continue;
					}
					else
					{
						Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap:  Create a new CUsbLinuxNode()"));
					}

					//
					// initialize the node from setup API information
					//
					USBD_NODE_ID nodeId = pUsbNode->Initialize((void *)device);
					/* if the node id was just processed then do not process again! */
					if( just_processed == nodeId) {
						Debug::trace("UsbManagerLinux.Attach", "Node Just processed: " +
								Hex::toString((long)nodeId) + "Release nodeid: " + Hex::toString((long)just_processed) );
						PAL_absclass::PAL_sleep(1);
						nodeId = 0;
					}
					else {
						just_processed = nodeId;
						if (!nodeId)
						{
							//
							// failed to initialize node
							//
							Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: failed to initialize node"));
							delete pUsbNode;
							pUsbNode = 0;
						}
						else if (!pUsbNode->isValid())
						{
							//
							// node is not valid
							//
							Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: invalid node"));
							delete pUsbNode;
							pUsbNode = 0;
						}
						else
						{
							CUsbNodeMap::iterator i = usbNodeMap.find(nodeId);
							Debug::trace("UsbManagerLinux.Attach", "NodeId:  " + Hex::toString((long)nodeId) + ",  Interface Class:  " + Integer::toString(pUsbNode->getInterfaceClass()) + ",  Protocol:  " + Integer::toString(pUsbNode->getDeviceProtocol()));
							if ( i == usbNodeMap.end())
							{
								Debug::trace("UsbManagerLinux.Attach", "DETECTED device. Adding to map.");
								Debug::trace("UsbManagerLinux.Attach", "Adding device, Interface Class:  " + Integer::toString(pUsbNode->getInterfaceClass()));
								Debug::trace("UsbManagerLinux.Attach", "Adding device, Device Protocol:  " + Integer::toString(pUsbNode->getDeviceProtocol()));

								// Device was detected, but node was not
								// found in map. Add node to the map
								//
	#if 1
								if( isDeviceAtmelSamba( getVendorId(nodeId), getProductId(nodeId) )) {
									if( addSamba ) {
										Debug::trace("UsbManagerLinux.Attach", "Samba device added to map: " + Hex::toString((long)nodeId));
										PAL_absclass::PAL_sleep(1);
										addToMap(nodeId, pUsbNode);
									}
									else {
										Debug::trace("UsbManagerLinux.Attach", "Samba device not added to map: " + Hex::toString((long)nodeId));
										PAL_absclass::PAL_sleep(1);
										nodeId = 0;
										delete pUsbNode;
										pUsbNode = 0;
									}
								}
								else {
									Debug::trace("UsbManagerLinux.Attach", "Node Added to Map: "+ Hex::toString((long)nodeId));
									Debug::trace("UsbManagerLinux.Attach", "Node Added to Map: "+ Hex::toString((long)getVendorId(nodeId)));
									if( getVendorId(nodeId) == ATMEL_SAMBA_VENDOR_ID )
									{
										if( addSamba ) {
											addToMap(nodeId, pUsbNode);
										}
										else
										{
											Debug::trace("UsbManagerLinux.Attach", "Do not attach yet!!");
											PAL_absclass::PAL_sleep(1);
											nodeId = 0;
											delete pUsbNode;
											pUsbNode = 0;

										}

									}
									else
									{
										addToMap(nodeId, pUsbNode);
									}
								}
	#else
								addToMap(nodeId, pUsbNode);
	#endif
								// Set initial state to DETECTED to indicate
								// the device has just been added.
								pUsbNode->state = CUsbNode::DETECTED;

								// set flag to prevent removal below
								pUsbNode->bDetected = true;
							}
							else
							{
								// Device was detected, but is already in map.
								Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: device already in map at node ") + Hex::toString((long)nodeId));
								// Delete the temp object and flag the node as detected.
								delete pUsbNode;
								pUsbNode = 0;
								// set flag to prevent removal below
								i->second->bDetected = true;
							}
						}
					}
				}
			}
			else
			{
				Debug::trace("UsbManagerLinux.Attach", "Device # " + Integer::toString(i) + " was not detected.");
			}
		}

		UsbCritSec.Leave();

		libusb_free_device_list(list,0);
    }
    /// libusb returned error!
	else
	{
		Debug::trace("UsbManagerLinux.Attach", "UpdateDeviceMap:  Error! libusb_get_device_list() returned a count of " + Integer::toString(cnt));
		return;
	}


    // Walk the map. Remove nodes that were not detected.
    // Launch updater/service classes if needed.
    //
    // pseudo code:
    //  if bDetected==false
    //      node was not detected in the scan we just performed above.
    //      handle cleanup/delete for associated updater or service class
    //      remove the node from the map
    //  else
    //      if node state == DETECTED
    //          if managed device
    //              if (needs update)
    //                  create updater
    //                  state = UPDATING
    //              else
    //                  create service class
    //                  state = RUNNING
    //          else
    //              state = UNMANAGED

	Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap:  Walk the map. Remove nodes that were not detected."));

	UsbCritSec.Enter();

RESCAN_USB_NODES:

    for ( CUsbNodeMap::iterator i( usbNodeMap.begin() ); i != usbNodeMap.end(); ++i )
    {
        CUsbNode *p( i->second );
        USBD_NODE_ID nodeId( i->first );

        if ( !p )
        {
            // node pointer was null, remove node from map
            Debug::trace("UsbManagerLinux.Attach",  HobString("UpdateDeviceMap: null pointer found at node ") + Hex::toString((ULONG)nodeId) + HobString(", removing node"));
            removeFromMap(nodeId);

            goto RESCAN_USB_NODES;
        }
        else if ( !p->bDetected )
		{
			const int nodeState( p->state );

			Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap:  Node ") + Hex::toString((long long)nodeId) + " was not detected. Remove from map!");

			// device in map was not detected
			// do some cleanup and remove from map
			if ( p->pUpdater )
			{
				DeleteUpdater(nodeId, p);
				Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap:  Deleted updater at node ") + Hex::toString((long long)nodeId));
			}

			if ( p->bServiceObject )
			{
				DeleteCommMgr(nodeId, p);
				Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap:  Deleted CommMgr at node ") + Hex::toString((long long)nodeId));
			}

			Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap:  Removing node ") + Hex::toString((long long)nodeId));
			removeFromMap(nodeId);

			if (nodeState == CUsbNode::STORAGE)
			{
#if 0
				if (keyFileState[KEY_FILE_TYPE_SERVICE] == KEY_FILE_STATE_FOUND)
					keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_REMOVED;
				if (keyFileState[KEY_FILE_TYPE_SUPERVISOR] == KEY_FILE_STATE_FOUND)
					keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_REMOVED;
printf("\n\n\nstorage dev removed\n\n\n\n");
#else
				EnableKeySearch(false);
#endif
			}
			else if (nodeState == CUsbNode::HID_KEYBOARD)
			{
				Debug::trace("UsbManagerLinux.Attach", "HID Keyboard device removed...");
				HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::KEYBOARD_REMOVED);

				foundKeyboard = false;

				dispatchEvent(&deviceEvent);
			}
			else if(nodeState == CUsbNode::HID_MOUSE)
			{
				Debug::trace("UsbManagerLinux.Attach", "HID Mouse device removed...");
				HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::MOUSE_REMOVED);

				foundMouse = false;

				dispatchEvent(&deviceEvent);

			}
			else if(nodeState == CUsbNode::HID_BARCODE)
			{
				Debug::trace("UsbManagerLinux.Attach", "HID barcode scanner device removed...");
				HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::BARCODE_SCANNER_REMOVED);

				foundBarcodeScanner = false;

				dispatchEvent(&deviceEvent);
			}
			else if(nodeState == CUsbNode::STORAGE)
			{
#if 0 /* detection of keys is now done within a python script since we cannot implement dbus to know when the kernel has mounted the filesystem -- ats */

				Debug::trace("UsbManagerLinux.KeySearch", "Storage device removed");
				if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SERVICE] ))
				{
					Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Service key removed.")  );
					if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE] )
					{
						Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Notifying system.")  );
						AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent);
						dispatchEvent(&deactive);
					}
					keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_REMOVED;
				}

				if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SUPERVISOR] ))
				{
					Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Supervisor key removed.")  );
					if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] )
					{
						Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: Notifying system.")  );
						AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent);
						dispatchEvent(&deactive);
					}
						keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_REMOVED;
				}

				if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_QATEST] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_QATEST] ))
				{
				   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: QATest key removed.")  );
				   if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_QATEST] )
				   {
					   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler:Notifying system ")  );
						// In the next line, I'm commenting out the flag for QATest because QA areed that the QATest flag does not need to be turned off
						// after it has been turned on (it will get turned off due to a reboot).  I still have to turn off the other two flags though.
					   AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent /* | AccessKeys::QATest */);
					   dispatchEvent(&deactive);
				   }
				   keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_REMOVED;
				}
				if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_MFGTEST] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_MFGTEST] ))
				{
				   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: MFGTest key removed.")  );
				   if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_MFGTEST] )
				   {
					   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler:Notifying system ")  );
					   AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent  | AccessKeys::MFGTest );
					   dispatchEvent(&deactive);
				   }
				   keyFileState[KEY_FILE_TYPE_MFGTEST] = KEY_FILE_STATE_REMOVED;
				}
				if ( ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SELFSERVE] )  || ( KEY_FILE_STATE_FOUND == keyFileState[KEY_FILE_TYPE_SELFSERVE] ))
				{
				   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler: SelfServePermanent key removed.")  );
				   if ( KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SELFSERVE] )
				   {
					   Debug::trace("UsbManager.KeySearch", HobString("KeySearchTimerHandler:Notifying system ")  );
					   AccessKeyDeactivation deactive(AccessKeys::SupervisorPermanent | AccessKeys::ServicePermanent  | AccessKeys::MFGTest | AccessKeys::SelfServePermanent );
					   dispatchEvent(&deactive);
				   }
				   keyFileState[KEY_FILE_TYPE_SELFSERVE] = KEY_FILE_STATE_REMOVED;
				}
#endif
			}
			else
			{
				Debug::trace("UsbManagerLinux.Attach", "HID device removed...");
			}

			goto RESCAN_USB_NODES;
		}
		else
		{
			// Node was detected in rescan above.
			Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap:  Node ") + Hex::toString((long long)nodeId) + " WAS detected.");

			if (p->state == CUsbNode::DETECTED)
			{
				// Node state = DETECTED means device was just added,
				// If the device PID is in our list of managed devices,
				// then create an updater or service class for it.

				Debug::trace("UsbManagerLinux.Attach", "UpdateDeviceMap: DETECTED:   ProductId = " + Hex::toString(getProductId(nodeId)) + ",  VendorId = " + Hex::toString(getVendorId(nodeId)));
				
				// SOF-2931
				if (isDeviceAtmelSamba(getVendorId(nodeId), getProductId(nodeId)))
				{
					addSamba = true;
					Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap(): addSamba=TRUE"));
				}
				
				if (isManagedDevice(getVendorId(nodeId), getProductId(nodeId)))
				{
#if 0
					bool updateRequired = isCodeUpdateRequired(getVendorId(nodeId), getProductId(nodeId));
					if (updateRequired && (getVendorId(nodeId) == HOBART_VENDOR_ID) && (getProductId(nodeId) == HOBART_EVO_WRAPPER_ID))
						updateRequired = false;
					if (updateRequired)
#else
					if (isCodeUpdateRequired(getVendorId(nodeId), getProductId(nodeId)))
#endif
					{
						// Code update is required for this device,
						// so create an updater for it. The updater
						// begins to run at the point it is constructed.

						Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: creating device updater for node ") + Hex::toString((long)nodeId));

						// Set the node state accordingly.
						p->state = CUsbNode::UPDATING;

						p->pUpdater = IUsbCodeUpdater::UpdaterFactory(nodeId);

						if (!p->pUpdater)
						{
							// Failed to create updater class for this device,
							// mark node as zombie. Zombie nodes are removed from
							// the map when the device removal notification comes in.
							Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: failed to create device updater for node ") + Hex::toString((long)nodeId));
							p->state = CUsbNode::ZOMBIE;
						}
					}
					else
					{
						// A firmware update is not required for this device.
						// Notify the RTC that this device was attached. RTC
						// launches the appropriate service class for the device.

						// Set the node state accordingly.
						p->state = CUsbNode::RUNNING;
						if (!AttachDevice( nodeId ))
						{
							// Failed to create service class for the device,
							// mark node as zombie. Zombie nodes are removed from
							// the map when the device removal notification comes in.
							Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: failed to create device service class for node ") + Hex::toString((long)nodeId));
							p->state = CUsbNode::ZOMBIE;
						}
					}
				}
				else if (getInterfaceClass(nodeId) == 8)
				{
					Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: adding storage device at node ") + Hex::toString((long)nodeId));
					p->state = CUsbNode::STORAGE;

#if 0 /* detection of keys is now done within a python script since we cannot implement dbus to know when the kernel has mounted the filesystem -- ats */

					/* TO DO: the sleep needs to be replaced with a call to dbus to determine when drive has
					 * been mounted and ready for access. --ats 05252013
					 */
					PAL_absclass::PAL_sleep(2250); //wait for thumb to mount before  activating key search
					EnableKeySearch(true);

#endif
				}
				else
				{
					p->show(); //added for debug
					//mouse and keyboard detection for fastrack
					if ( getInterfaceClass(nodeId) == 3 )  //Human Interface Devices
					{
						//ignore our touchscreen device
						if( getVendorId(nodeId) != TOUCHSCREEN_PRODUCT_ID ) {
							if(getDeviceProtocol(nodeId) == 2)
							{
								Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: adding mouse to map ") + Hex::toString((long)nodeId));
								p->state = CUsbNode::HID_MOUSE;
								HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::MOUSE_ATTACHED);

								foundMouse = true;

								//notify the session of the event
								dispatchEvent(&deviceEvent);
							}
							else
							{
								if(getiInterface(nodeId) != 5)
								{
									Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: adding keyboard to map ") + Hex::toString((long)nodeId));
									p->state = CUsbNode::HID_KEYBOARD;
									HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::KEYBOARD_ATTACHED);

									foundKeyboard = true;

									//notify the session of the event
									dispatchEvent(&deviceEvent);

								}
								else
								{
									Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: adding barcode scanner to map ") + Hex::toString((long)nodeId));
									p->state = CUsbNode::HID_BARCODE;
									HIDDeviceEvent deviceEvent(this, HIDDeviceEvent::BARCODE_SCANNER_ATTACHED);

									foundBarcodeScanner = true;

									//notify the session of the event
									dispatchEvent(&deviceEvent);

								}

							}
						}
					}
					else
					{
						//  not a managed device, nothing to do
						Debug::trace("UsbManagerLinux.Attach", HobString("UpdateDeviceMap: adding unmanaged device at node ") + Hex::toString((long)nodeId));
						p->state = CUsbNode::UNMANAGED;
					}
				}
			}
		}

    } // for iterator

    UsbCritSec.Leave();
}
#else 
void UsbManagerLinux::UpdateDeviceMap()
{
	
}
#endif


// Search all removable drives for key files.
// If a key file is found, change its state to FOUND.
bool UsbManagerLinux::searchForKeyFiles()
{
    bool bFound = false, serviceKey = false, manKey = false, selfKey = false;
    bool superKey = false, qaKey = false;

    Debug::trace("UsbManagerLinux.KeySearch", "Search for keys");
    // TODO:
    // I have hardcoded the paths below to python and verifyDongleKey but this needs changed to using
    // methods to get from internal configuration where python and scripts live
    // Windows version of this is the same except call ShellExecA instead of system
    
#if 1 //new fastrack key search method
    int result = system( "/usr/bin/python /usr/local/hobart/python/verifyDongleKey.pyc service" );
    Debug::trace("UsbManagerLinux.keysearch", "Result: " + Integer::toString(result));
    if( result == 0)
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Found Service key");
    	keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_FOUND;
    	serviceKey = true;
    }
    else
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Service key not found");
    	keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_UNKNOWN;
    	serviceKey = false;
    }

    result = system( "/usr/bin/python /usr/local/hobart/python/verifyDongleKey.pyc manufacturing" );
    if( result == 0)
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Found Manufacturing key");
    	keyFileState[KEY_FILE_TYPE_MFGTEST] = KEY_FILE_STATE_FOUND;
    	manKey = true;
	}
    else
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Manufacturing key not found");
    	keyFileState[KEY_FILE_TYPE_MFGTEST] = KEY_FILE_STATE_UNKNOWN;
    	manKey = false;
    }

    result = system( "/usr/bin/python /usr/local/hobart/python/verifyDongleKey.pyc selfservice" );
    if( result == 0)
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Found Selfservice key");
    	keyFileState[KEY_FILE_TYPE_SELFSERVE] = KEY_FILE_STATE_FOUND;
    	selfKey = true;
	}
    else
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Selfservice key not found");
    	keyFileState[KEY_FILE_TYPE_SELFSERVE] = KEY_FILE_STATE_UNKNOWN;
    	selfKey = false;
    }

    result = system( "/usr/bin/python /usr/local/hobart/python/verifyDongleKey.pyc supervisor" );
    if( result == 0)
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Found Supervisor key");
    	keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_FOUND;
    	superKey = true;
	}
    else
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Supervisor key not found");
    	keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_UNKNOWN;
    	superKey = false;
    }

    result = system( "/usr/bin/python /usr/local/hobart/python/verifyDongleKey.pyc systemtest" );
    if( result == 0)
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Found Systemtest key");
    	keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_FOUND;
    	qaKey = true;
	}
    else
    {
    	Debug::trace("UsbManagerLinux.keysearch", "Systemtest key not found");
    	keyFileState[KEY_FILE_TYPE_QATEST] =  KEY_FILE_STATE_UNKNOWN;
    	qaKey = false;
    }

	Debug::trace("UsbManagerLinux.keysearch", HobString("search Key State Service: ")  + Integer::toString(keyFileState[KEY_FILE_TYPE_SERVICE]) );
	Debug::trace("UsbManagerLinux.keysearch", HobString("search Key State Super: ")  + Integer::toString(keyFileState[KEY_FILE_TYPE_SUPERVISOR]) );
	Debug::trace("UsbManagerLinux.keysearch", HobString("search Key State Qa: ")  + Integer::toString(keyFileState[KEY_FILE_TYPE_QATEST]) );
	Debug::trace("UsbManagerLinux.keysearch", HobString("search Key State SelfServe: ")  + Integer::toString(keyFileState[KEY_FILE_TYPE_SELFSERVE]) );
	Debug::trace("UsbManagerLinux.keysearch", HobString("search Key State Manufacturing: ")  + Integer::toString(keyFileState[KEY_FILE_TYPE_MFGTEST]) );


    bFound = (serviceKey | manKey | selfKey | superKey | qaKey);
#else
    std::string usbPath = Configuration::getInstance()->toStringCfgI(CfgInternal::KEYMOUNTDIR);

    {
	    string path="/media/";
            vector<string> thumb_drvs = PAL_absclass::PAL_getDirectoryList(path.c_str());
            std::vector<std::string>::iterator drvs_idx;
            for(drvs_idx=thumb_drvs.begin(); drvs_idx!=thumb_drvs.end();drvs_idx++) 
            {
                std::string drv= path+*drvs_idx;
                Debug::trace("UsbManagerLinux.keysearch", HobString(drv.c_str()));
		//
		//dont search cdrom directories to save time. We have a problem if someone
		//names their thumbdrive cdrom something or the other! 
		//
//		if (0 == (drv.find("cdrom")-path.length()+1))
//			continue;
///
                //search
#ifdef USB_DISPLAY_THUMB_CONTENTS
                vector<string> dirlist = PAL_absclass::PAL_getDirectoryList(drv.c_str());
                std::vector<std::string>::iterator w;

				for(w = dirlist.begin(); w != dirlist.end(); w++) 
                {
					std::string file = *w;
					Debug::trace("UsbManagerLinux.keysearch", HobString(file.c_str()));
                }
#endif      // USB_DISPLAY_THUMB_CONTENTS

                File serviceKeyFile(drv.c_str(), Configuration::getInstance()->getServiceKeyFile());

                Debug::trace("UsbManagerLinux.keysearch", HobString("searchForKeyFiles: searching for service key at ") + serviceKeyFile.getFullPath() );

                if (checkForKeyValue(serviceKeyFile))
                {
					/*we want to send the key found state even if we are in notified state
					  as the UI may have missed a notification */
                    if ( KEY_FILE_STATE_UNKNOWN == keyFileState[KEY_FILE_TYPE_SERVICE]
                            || KEY_FILE_STATE_REMOVED == keyFileState[KEY_FILE_TYPE_SERVICE] 
							|| KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE])
                    {
                        if ( serviceKeyFile.size() == 42 )
                        {
                            Debug::trace("UsbManager.KeySearch", HobString("searchForKeyFiles: found service key at ") + serviceKeyFile.getFullPath() );
                            keyFileState[KEY_FILE_TYPE_QATEST] = KEY_FILE_STATE_FOUND;
                            bFound = true;
                        }
                        else
                        {
                            Debug::trace("UsbManager.KeySearch", HobString("searchForKeyFiles: found service key at ") + serviceKeyFile.getFullPath() );
							keyFileState[KEY_FILE_TYPE_SERVICE] = KEY_FILE_STATE_FOUND;
                            bFound = true;
                        }
                    }
		    else
                        Debug::trace("UsbManagerLinux.keysearch", HobString("searchForKeyFiles: Service key at ") + serviceKeyFile.getFullPath() +
									HobString("already in records.") );			
                }

                File superKeyFile(drv.c_str(), Configuration::getInstance()->getSuperKeyFile());

                Debug::trace("UsbManagerLinux.keysearch", HobString("searchForKeyFiles: searching for supervisor key at ") + superKeyFile.getFullPath() );

                if (checkForKeyValue(superKeyFile))
                {
					/*we want to send the key found state even if we are in notified state
					  as the UI may have missed a notification */
                    if ( KEY_FILE_STATE_UNKNOWN == keyFileState[KEY_FILE_TYPE_SUPERVISOR]
                            || KEY_FILE_STATE_REMOVED == keyFileState[KEY_FILE_TYPE_SUPERVISOR] 
							|| KEY_FILE_STATE_NOTIFIED == keyFileState[KEY_FILE_TYPE_SERVICE])
                    {
                        Debug::trace("UsbManagerLinux.keysearch", HobString("searchForKeyFiles: found supervisor key at ") + superKeyFile.getFullPath() );
                        keyFileState[KEY_FILE_TYPE_SUPERVISOR] = KEY_FILE_STATE_FOUND;
                        bFound = true;
                    }
		    else
                        Debug::trace("UsbManagerLinux.keysearch", HobString("searchForKeyFiles: Supervisor key at ") + superKeyFile.getFullPath() +
									HobString("already in records.") );
                }

#if 0	//	ToDo: This isn't working. Fix at a later date.
#ifdef USE_ACCOUNTANT
                File accountantKeyFile(servDir.getFullPath(), "accountant\/accountant.dat");

                if (checkForKeyValue(accountantKeyFile))
                {
                    util_acc::showInstanceCounters(false);
                }
#endif
#endif

#ifdef USB_TRIGGER_MULTIPLE_TEST_LABELS
                File labelTriggerKeyFile(drv.c_str(), "labelTrigger/key.dat");

                if (checkForKeyValue(labelTriggerKeyFile))
                {
                    static int toggle = 0;

                    Debug::trace("UsbManagerLinux.keysearch", HobString("searchForKeyFiles: found label trigger key at ") + labelTriggerKeyFile.getFullPath() );

                    if ((toggle % 2) == 0)
                    {
                        PrinterPrintTestLabel testLabel(this, FirstPrinter, PrVerticalLines, PrStartContinuous);
                        dispatchEvent(&testLabel);
                    }
                    else
                    {
                        PrinterPrintTestLabel testLabel(this, FirstPrinter, PrVerticalLines, PrStopContinuous);
                        dispatchEvent(&testLabel);
                    }
                    toggle++;
                }
#endif
            }
        Debug::trace("UsbManagerLinux.keysearch", HobString("searchForKeyFiles: search pass complete. ") );
    }
#endif
    return bFound;
}


// Check if there is any USB Storage Device (thumb drive) present in the system.
// Need this method because we don't want to shutdown with one present,
// since the scale will try to boot from it the next time it boots.
bool UsbManagerLinux::isBulkStorageDevicePresent() const
{
    libusb_device **list;
    //list of devices currently attached to the system.
    ssize_t cnt = libusb_get_device_list(NULL, &list);
	Debug::trace("UsbManagerLinux.Attach", "isBulkStorageDevicePresent(): libusb_get_device_list() returned a count of " + Integer::toString(cnt));
    ssize_t i = 0;

    for (i=0; i<cnt; i++)
    {
        libusb_device *device = list[i];
        libusb_device_descriptor desc;
        //get the device descriptor for the current device
        if (libusb_get_device_descriptor(device, &desc) == 0)
        {
            libusb_config_descriptor *cdesc = (libusb_config_descriptor *)malloc(sizeof(libusb_config_descriptor) * desc.bNumConfigurations);
            if (cdesc)
            {
                for (int i = 0; i < desc.bNumConfigurations; i++)
                {
                    if (libusb_get_config_descriptor(device, i, &cdesc) == 0)
					{
                        if (cdesc->interface[0].altsetting[0].bInterfaceClass == 8)
						{
                            delete cdesc;
                            return true;
						}
					}
                }
                delete cdesc;
            }
        }
    }
    return false;  
}

// SOF-2931
bool UsbManagerLinux::isSambaPresent() const
{
	return addSamba;
}

//void UsbManagerLinux::resetSamba() const
void UsbManagerLinux::resetSamba()
{
	addSamba = false;
	Debug::trace("UsbManagerLinux.Attach", HobString("resetSamba(): addSamba=FALSE"));
}
