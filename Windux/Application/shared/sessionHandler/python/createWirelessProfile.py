#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys, os, string
import shutil
from optparse import OptionParser

def LEAPorPEAP(isPeap,EAPUserCredentialsFile, username, password, ssid):
    if isPeap:
        XMLUserCredentials =  '<?xml version="1.0"?>\n'
        XMLUserCredentials += '<EapHostUserCredentials \n'
        XMLUserCredentials += '    xmlns="http://www.microsoft.com/provisioning/EapHostUserCredentials"\n'
        XMLUserCredentials += '    xmlns:eapCommon="http://www.microsoft.com/provisioning/EapCommon"\n'
        XMLUserCredentials += '    xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapMethodUserCredentials">\n'
        XMLUserCredentials += '  <EapMethod>\n'
        XMLUserCredentials += '    <eapCommon:Type>25</eapCommon:Type>\n'
        XMLUserCredentials += '    <eapCommon:AuthorId>0</eapCommon:AuthorId>\n'
        XMLUserCredentials += '  </EapMethod>\n'
        XMLUserCredentials += '  <Credentials xmlns:eapUser="http://www.microsoft.com/provisioning/EapUserPropertiesV1"\n'
        XMLUserCredentials += '            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"\n'
        XMLUserCredentials += '            xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapUserPropertiesV1"\n'
        XMLUserCredentials += '            xmlns:MsPeap="http://www.microsoft.com/provisioning/MsPeapUserPropertiesV1"\n'
        XMLUserCredentials += '            xmlns:MsChapV2="http://www.microsoft.com/provisioning/MsChapV2UserPropertiesV1">\n'
        XMLUserCredentials += '    <baseEap:Eap>\n'
        XMLUserCredentials += '      <baseEap:Type>25</baseEap:Type>\n'
        XMLUserCredentials += '      <MsPeap:EapType>\n'
        XMLUserCredentials += '         <MsPeap:RoutingIdentity>' + ssid + '</MsPeap:RoutingIdentity>\n'
        XMLUserCredentials += '         <baseEap:Eap>\n'
        XMLUserCredentials += '            <baseEap:Type>26</baseEap:Type>\n'
        XMLUserCredentials += '            <MsChapV2:EapType>\n'
        XMLUserCredentials += '               <MsChapV2:Username>' + username + '</MsChapV2:Username>\n'
        XMLUserCredentials += '               <MsChapV2:Password>' + password + '</MsChapV2:Password>\n'
        XMLUserCredentials += '            </MsChapV2:EapType>\n'
        XMLUserCredentials += '         </baseEap:Eap>\n'
        XMLUserCredentials += '      </MsPeap:EapType>\n'
        XMLUserCredentials += '    </baseEap:Eap>\n'
        XMLUserCredentials += '  </Credentials>\n'
        XMLUserCredentials += '</EapHostUserCredentials>\n'
    elif not isPeap:
        XMLUserCredentials =  '<?xml version="1.0"?>\n'
        XMLUserCredentials += '<EapHostUserCredentials \n'
        XMLUserCredentials += '    xmlns="http://www.microsoft.com/provisioning/EapHostUserCredentials"\n'
        XMLUserCredentials += '    xmlns:eapCommon="http://www.microsoft.com/provisioning/EapCommon"\n'
        XMLUserCredentials += '    xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapMethodUserCredentials">\n'
        XMLUserCredentials += '  <EapMethod>\n'
        XMLUserCredentials += '    <eapCommon:Type>17</eapCommon:Type>\n'
        XMLUserCredentials += '    <eapCommon:AuthorId>0</eapCommon:AuthorId>\n'
        XMLUserCredentials += '  </EapMethod>\n'
        XMLUserCredentials += '  <Credentials xmlns:eapUser="http://www.microsoft.com/provisioning/EapUserPropertiesV1"\n'
        XMLUserCredentials += '            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"\n'
        XMLUserCredentials += '            xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapUserPropertiesV1"\n'
        XMLUserCredentials += '            xmlns:MsPeap="http://www.microsoft.com/provisioning/MsPeapUserPropertiesV1"\n'
        XMLUserCredentials += '            xmlns:MsChapV2="http://www.microsoft.com/provisioning/MsChapV2UserPropertiesV1">\n'
        XMLUserCredentials += '    <baseEap:Eap>\n'
        XMLUserCredentials += '      <baseEap:Type>17</baseEap:Type>\n'
        '''XMLUserCredentials += '      <MsPeap:EapType>\n'
        XMLUserCredentials += '         <MsPeap:RoutingIdentity>' + ssid + '</MsPeap:RoutingIdentity>\n'
        XMLUserCredentials += '         <baseEap:Eap>\n'
        XMLUserCredentials += '            <baseEap:Type>26</baseEap:Type>\n'
        XMLUserCredentials += '            <MsChapV2:EapType>\n'
        XMLUserCredentials += '               <MsChapV2:Username>' + username + '</MsChapV2:Username>\n'
        XMLUserCredentials += '               <MsChapV2:Password>' + password + '</MsChapV2:Password>\n'
        XMLUserCredentials += '            </MsChapV2:EapType>\n'
        XMLUserCredentials += '         </baseEap:Eap>\n'
        XMLUserCredentials += '      </MsPeap:EapType>\n'
        '''
        XMLUserCredentials += '    </baseEap:Eap>\n'
        XMLUserCredentials += '  </Credentials>\n'
        XMLUserCredentials += '</EapHostUserCredentials>\n'

    if isPeap:
        if os.path.isfile(EAPUserCredentialsFile):
            try:
                os.remove(EAPUserCredentialsFile)
            except:
                pass

        # CREATE AND WRITE EXISTING PROFILE FILE
        try:
            f = open(EAPUserCredentialsFile,  'w')
            f.write(XMLUserCredentials)
            f.close()
            pass
        except:
            pass

def writeXML(wirelessProfileXMLFile, EAPUserCredentialsFile, ssidText, encryption, securityMethod, keyType,  passphrase, defaultKey,  Key1,  Key2,  Key3,  Key4, username, password):
    #print ssidText
    firstText = '<?xml version="1.0"?>\n<WLANProfile xmlns="http://www.microsoft.com/networking/WLAN/profile/v1">\n    <name>WirelessProfile</name>\n    <SSIDConfig>\n		<SSID>\n			<hex>'
    
    nextText = '</hex>\n            <name>'
    secondText = '</name>\n		</SSID>\n		<nonBroadcast>true</nonBroadcast>\n    </SSIDConfig>\n	<connectionType>ESS</connectionType>\n	<connectionMode>auto</connectionMode>\n    <autoSwitch>false</autoSwitch>\n	<MSM>\n		<security>\n			<authEncryption>\n			    <authentication>'
    thirdText = '            </authEncryption>\n'
    endText = '	   </security>\n    </MSM>\n    </WLANProfile>'
    
    if securityMethod == "OPEN":
        securityMethod = "open"
    if encryption == "None":
        encryption = "none"
    XMLFileContents =  firstText + ssidText.encode("hex") + nextText + ssidText + secondText + securityMethod + '</authentication>\n'
    XMLFileContents += '			    <encryption>' + encryption + '</encryption>\n'

    if keyType == 'PEAP':
        XMLFileContents += '                <useOneX>true</useOneX>\n'
        
    elif keyType == 'LEAP':
        XMLFileContents += '                <useOneX>true</useOneX>\n'

    else:
        XMLFileContents += '                <useOneX>false</useOneX>\n'

    if securityMethod != 'open':
        XMLFileContents += '               <FIPSMode xmlns="http://www.microsoft.com/networking/WLAN/profile/v2">false</FIPSMode>\n'

    XMLFileContents += thirdText

    if encryption == "none" and securityMethod == "open":
        pass
    elif keyType == 'networkKey':
        #import configs to get network keys

        XMLFileContents += '<sharedKey>\n				<keyType>' + keyType + '</keyType>\n				<protected>false</protected>\n                <keyMaterial>'
        if securityMethod == 'open':
            if Key1 == "0":
                Key1 = ""
            if Key2 == "0":
                Key2 = ""
            if Key3 == "0":
                Key3 = ""
            if Key4 == "0":
                Key4 = ""

            if defaultKey == "1":
                XMLFileContents += Key1 + '</keyMaterial>\n'
                XMLFileContents += '</sharedKey>\n       <keyIndex>0</keyIndex>\n'
                pass
            elif defaultKey == "2":
                XMLFileContents += Key2 + '</keyMaterial>\n'
                XMLFileContents += '</sharedKey>\n       <keyIndex>1</keyIndex>\n'
                pass
            elif defaultKey == "3":
                XMLFileContents += Key3 + '</keyMaterial>\n'
                XMLFileContents += '</sharedKey>\n       <keyIndex>2</keyIndex>\n'
                pass
            elif defaultKey == "4":
                XMLFileContents += Key4 + '</keyMaterial>\n'
                XMLFileContents += '</sharedKey>\n       <keyIndex>3</keyIndex>\n'
                pass
        else:
            XMLFileContents += passphrase + '</keyMaterial>\n     </sharedKey>\n'
        #XMLFileContents += '            </sharedKey>\n'
    elif keyType == 'passPhrase':
        XMLFileContents += '<sharedKey>\n				<keyType>' + keyType + '</keyType>\n				<protected>false</protected>\n                <keyMaterial>'
        XMLFileContents += passphrase + '</keyMaterial>\n             </sharedKey>\n'
    elif keyType == 'PEAP':


        XMLFileContents  = '<?xml version="1.0" encoding="US-ASCII"?>\n'
        XMLFileContents += '<WLANProfile xmlns="http://www.microsoft.com/networking/WLAN/profile/v1">\n'
        XMLFileContents += '    <name>WirelessProfile</name>\n'
        XMLFileContents += '    <SSIDConfig>\n'
        XMLFileContents += '        <hex>' + ssidText.encode("hex") + '</hex>\n'
        XMLFileContents += '        <SSID>\n'
        XMLFileContents += '            <name>' + ssidText + '</name>\n'
        XMLFileContents += '        </SSID>\n'
        XMLFileContents += '        <nonBroadcast>true</nonBroadcast>\n'
        XMLFileContents += '    </SSIDConfig>\n'
        XMLFileContents += '    <connectionType>ESS</connectionType>\n'
        XMLFileContents += '    <connectionMode>auto</connectionMode>\n'
        XMLFileContents += '    <autoSwitch>false</autoSwitch>\n'
        XMLFileContents += '    <MSM>\n'
        XMLFileContents += '        <security>\n'
        XMLFileContents += '            <authEncryption>\n'
        XMLFileContents += '                <authentication>'

        #print securityMethod
        if securityMethod == "WPA2-Enterprise (PEAP)":
            #print "TODD"
            XMLFileContents += 'WPA2</authentication>\n'
        elif securityMethod == "WPA-Enterprise (PEAP)":
            #print "WOOZ"
            XMLFileContents += 'WPA</authentication>\n'

        XMLFileContents += '                <encryption>' + encryption + '</encryption>\n'
        XMLFileContents += '                <useOneX>true</useOneX>\n'
        XMLFileContents += '            </authEncryption>\n'

        if securityMethod == "WPA2-Enterprise (PEAP)":
            XMLFileContents += '           <PMKCacheMode>enabled</PMKCacheMode>\n'
            XMLFileContents += '           <PMKCacheTTL>720</PMKCacheTTL>\n'
            XMLFileContents += '           <PMKCacheSize>128</PMKCacheSize>\n'
            XMLFileContents += '           <preAuthMode>disabled</preAuthMode>\n'

        XMLFileContents += '            <OneX xmlns="http://www.microsoft.com/networking/OneX/v1">\n'
        XMLFileContents += '              <cacheUserData>true</cacheUserData>\n'
        XMLFileContents += '              <authMode>user</authMode>\n'
        XMLFileContents += '                <EAPConfig>\n'
        XMLFileContents += '                    <EapHostConfig xmlns="http://www.microsoft.com/provisioning/EapHostConfig" xmlns:eapCommon="http://www.microsoft.com/provisioning/EapCommon" xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapMethodConfig">\n'
        XMLFileContents += '                        <EapMethod>\n'
        XMLFileContents += '                            <eapCommon:Type>25</eapCommon:Type>\n'
        XMLFileContents += '                            <eapCommon:AuthorId>0</eapCommon:AuthorId>\n'
        XMLFileContents += '                        </EapMethod>\n'
        XMLFileContents += '                        <Config xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapConnectionPropertiesV1"'
        XMLFileContents += '                                xmlns:msPeap="http://www.microsoft.com/provisioning/MsPeapConnectionPropertiesV1"'
        XMLFileContents += '                                xmlns:msChapV2="http://www.microsoft.com/provisioning/MsChapV2ConnectionPropertiesV1">\n'
        XMLFileContents += '                             <baseEap:Eap>\n'
        XMLFileContents += '                                 <baseEap:Type>25</baseEap:Type>\n'
        XMLFileContents += '                                 <msPeap:EapType>\n'

        if securityMethod == "WPA2-Enterprise (PEAP)":
            XMLFileContents += '                                     <msPeap:FastReconnect>true</msPeap:FastReconnect>\n'

        XMLFileContents += '                                     <msPeap:InnerEapOptional>0</msPeap:InnerEapOptional>\n'
        XMLFileContents += '                                     <baseEap:Eap>\n'
        XMLFileContents += '                                         <baseEap:Type>26</baseEap:Type>\n'
        XMLFileContents += '                                         <msChapV2:EapType>\n'
        XMLFileContents += '                                             <msChapV2:UseWinLogonCredentials>false</msChapV2:UseWinLogonCredentials>\n'
        XMLFileContents += '                                         </msChapV2:EapType>\n'
        XMLFileContents += '                                     </baseEap:Eap>\n'
        XMLFileContents += '                                     <msPeap:EnableQuarantineChecks>false</msPeap:EnableQuarantineChecks>\n'
        XMLFileContents += '                                     <msPeap:RequireCryptoBinding>false</msPeap:RequireCryptoBinding>\n'
        XMLFileContents += '                                     <msPeap:PeapExtensions />\n'
        XMLFileContents += '                                  </msPeap:EapType>\n'
        XMLFileContents += '                             </baseEap:Eap>\n'
        XMLFileContents += '                        </Config>\n'
        XMLFileContents += '                    </EapHostConfig>\n'
        XMLFileContents += '                </EAPConfig>\n'
        XMLFileContents += '            </OneX>\n'

        LEAPorPEAP(True, EAPUserCredentialsFile, username, password, ssidText)

    elif keyType == 'LEAP':

        XMLFileContents  = '<?xml version="1.0" encoding="US-ASCII"?>\n'
        XMLFileContents += '<WLANProfile xmlns="http://www.microsoft.com/networking/WLAN/profile/v1">\n'
        XMLFileContents += '    <name>WirelessProfile</name>\n'
        XMLFileContents += '    <SSIDConfig>\n'
        XMLFileContents += '        <SSID>\n'
        XMLFileContents += '            <name>' + ssidText + '</name>\n'
        XMLFileContents += '        </SSID>\n'
        XMLFileContents += '        <nonBroadcast>true</nonBroadcast>\n'
        XMLFileContents += '    </SSIDConfig>\n'
        XMLFileContents += '    <connectionType>ESS</connectionType>\n'
        XMLFileContents += '    <connectionMode>auto</connectionMode>\n'
        XMLFileContents += '    <autoSwitch>false</autoSwitch>\n'
        XMLFileContents += '    <MSM>\n'
        XMLFileContents += '        <security>\n'
        XMLFileContents += '            <authEncryption>\n'
        XMLFileContents += '                <authentication>'

        #print securityMethod
        if securityMethod == "WPA2-Enterprise (LEAP)":
            #print "TODD"
            XMLFileContents += 'WPA2</authentication>\n'
        elif securityMethod == "WPA-Enterprise (LEAP)":
            #print "WOOZ"
            XMLFileContents += 'WPA</authentication>\n'

        XMLFileContents += '                <encryption>' + encryption + '</encryption>\n'
        XMLFileContents += '                <useOneX>true</useOneX>\n'
        XMLFileContents += '            </authEncryption>\n'


        XMLFileContents += '            <OneX xmlns="http://www.microsoft.com/networking/OneX/v1">\n'

        XMLFileContents += '                <EAPConfig>\n'
        XMLFileContents += '                    <EapHostConfig xmlns="http://www.microsoft.com/provisioning/EapHostConfig" xmlns:eapCommon="http://www.microsoft.com/provisioning/EapCommon" xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapMethodConfig">\n'
        XMLFileContents += '                        <EapMethod>\n'
        XMLFileContents += '                            <eapCommon:Type>17</eapCommon:Type>\n'
        XMLFileContents += '                            <eapCommon:AuthorId>0</eapCommon:AuthorId>\n'
        XMLFileContents += '                        </EapMethod>\n'
        XMLFileContents += '                        <Config xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapConnectionPropertiesV1"'
        XMLFileContents += '                                xmlns:msPeap="http://www.microsoft.com/provisioning/MsPeapConnectionPropertiesV1"'
        XMLFileContents += '                                xmlns:msChapV2="http://www.microsoft.com/provisioning/MsChapV2ConnectionPropertiesV1">\n'
        XMLFileContents += '                             <baseEap:Eap>\n'
        XMLFileContents += '                                 <baseEap:Type>25</baseEap:Type>\n'
        XMLFileContents += '                                 <msPeap:EapType>\n'
        XMLFileContents += '                                     <msPeap:FastReconnect>true</msPeap:FastReconnect>\n'
        XMLFileContents += '                                     <msPeap:InnerEapOptional>0</msPeap:InnerEapOptional>\n'
        XMLFileContents += '                                     <baseEap:Eap>\n'
        XMLFileContents += '                                         <baseEap:Type>26</baseEap:Type>\n'
        XMLFileContents += '                                         <msChapV2:EapType>\n'
        XMLFileContents += '                                             <msChapV2:UseWinLogonCredentials>false</msChapV2:UseWinLogonCredentials>\n'
        XMLFileContents += '                                         </msChapV2:EapType>\n'
        XMLFileContents += '                                     </baseEap:Eap>\n'
        XMLFileContents += '                                     <msPeap:EnableQuarantineChecks>false</msPeap:EnableQuarantineChecks>\n'
        XMLFileContents += '                                     <msPeap:RequireCryptoBinding>false</msPeap:RequireCryptoBinding>\n'
        XMLFileContents += '                                     <msPeap:PeapExtensions />\n'
        XMLFileContents += '                                  </msPeap:EapType>\n'
        XMLFileContents += '                             </baseEap:Eap>\n'
        XMLFileContents += '                        </Config>\n'
        XMLFileContents += '                    </EapHostConfig>\n'
        XMLFileContents += '                </EAPConfig>\n'
        XMLFileContents += '            </OneX>\n'
        isPeap = False
        LEAPorPEAP(isPeap, EAPUserCredentialsFile, username, password, ssidText)

    elif keyType == "802PEAP" or keyType == "802LEAP":
        #ssidText.encode("hex")
        isPeap = True
        XMLFileContents  = '<?xml version="1.0"?>\n'
        XMLFileContents += '<WLANProfile xmlns="http://www.microsoft.com/networking/WLAN/profile/v1">\n'
        XMLFileContents += '    <name>WirelessProfile</name>\n'
        XMLFileContents += '    <SSIDConfig>\n'
        XMLFileContents += '        <SSID>\n'
        XMLFileContents += '            <hex>'
        XMLFileContents += ssidText.encode("hex")
        XMLFileContents += '</hex>\n'
        XMLFileContents += '            <name>' + ssidText + '</name>\n'
        XMLFileContents += '        </SSID>\n'
        XMLFileContents += '        <nonBroadcast>true</nonBroadcast>\n'
        XMLFileContents += '    </SSIDConfig>\n'
        XMLFileContents += '    <connectionType>ESS</connectionType>\n'
        XMLFileContents += '    <connectionMode>auto</connectionMode>\n'
        XMLFileContents += '    <autoSwitch>false</autoSwitch>\n'
        XMLFileContents += '    <MSM>\n'
        XMLFileContents += '        <security>\n'
        XMLFileContents += '            <authEncryption>\n'
        XMLFileContents += '                <authentication>'

        #print securityMethod
        if securityMethod == "802.1X (PEAP)" or securityMethod == "802.1X (LEAP)":
            #print "TODD"
            XMLFileContents += 'open</authentication>\n'
            XMLFileContents += '                <encryption>WEP</encryption>\n'


        XMLFileContents += '                <useOneX>true</useOneX>\n'
        XMLFileContents += '            </authEncryption>\n'

        XMLFileContents += '           <PMKCacheMode>enabled</PMKCacheMode>\n'
        XMLFileContents += '           <PMKCacheTTL>720</PMKCacheTTL>\n'
        XMLFileContents += '           <PMKCacheSize>128</PMKCacheSize>\n'
        XMLFileContents += '           <preAuthMode>disabled</preAuthMode>\n'

        XMLFileContents += '            <OneX xmlns="http://www.microsoft.com/networking/OneX/v1">\n'
        XMLFileContents += '              <cacheUserData>true</cacheUserData>\n'
        XMLFileContents += '              <authMode>user</authMode>\n'

        XMLFileContents += '                <EAPConfig>\n'
        XMLFileContents += '                    <EapHostConfig xmlns="http://www.microsoft.com/provisioning/EapHostConfig">\n'
        XMLFileContents += '                        <EapMethod>\n'

        if securityMethod == "802.1X (PEAP)":
            isPeap = True
            XMLFileContents += '                            <Type xmlns="http://www.microsoft.com/provisioning/EapCommon">25</Type>\n'
        elif securityMethod == "802.1X (LEAP)":
            isPeap = False
            XMLFileContents += '                            <Type xmlns="http://www.microsoft.com/provisioning/EapCommon">17</Type>\n'

        '''
        if securityMethod == "802.1X (LEAP)":
            XMLFileContents += '                             <VendorId xmlns="http://www.microsoft.com/provisioning/EapCommon">0</VendorId>\n'
            XMLFileContents += '                             <VendorType xmlns="http://www.microsoft.com/provisioning/EapCommon">0</VendorType>\n'
            XMLFileContents += '                             <AuthorId xmlns="http://www.microsoft.com/provisioning/EapCommon">9</AuthorId>\n'
            XMLFileContents += '                        </EapMethod>\n'
            XMLFileContents += '                        <Config xmlns="http://www.microsoft.com/provisioning/EapHostConfig">\n'
            XMLFileContents += '                           <eapLeap xmlns="http://www.cisco.com/CCX"><unprotectedIdentityPattern encryptContent="true">\n'
            XMLFileContents += '                              <EncryptedData xmlns="http://www.w3.org/2001/04/xmlenc#" Type="http://www.w3.org/2001/04/xmlenc#Content">\n'
            XMLFileContents += '                                 <EncryptionMethod Algorithm="http://www.w3.org/2001/04/xmlenc#aes256-cbc"/>\n'
            XMLFileContents += '                                     <CipherData>\n'
            XMLFileContents += '                                         <CipherValue>FIKqmqcVD8KDFHW9SNPdJzrtBtH85ogcdKQplBP6E2c=</CipherValue>\n'
            XMLFileContents += '                                     </CipherData>\n'
            XMLFileContents += '                                 </EncryptedData>\n'
            XMLFileContents += '                                 </unprotectedIdentityPattern>\n'
            XMLFileContents += '                                 <passwordSource>\n'
            XMLFileContents += '                                      <passwordFromProfile encryptContent="true">\n'
            XMLFileContents += '                                          <EncryptedData xmlns="http://www.w3.org/2001/04/xmlenc#" Type="http://www.w3.org/2001/04/xmlenc#Content">\n'
            XMLFileContents += '                                          <EncryptionMethod Algorithm="http://www.w3.org/2001/04/xmlenc#aes256-cbc"/>\n'
            XMLFileContents += '                                              <CipherData>\n'
            XMLFileContents += '                                                  <CipherValue>dybr8IWG5kAaDg7pPzT6IketbBNj13xAQ4JTWSmmqWU=</CipherValue>\n'
            XMLFileContents += '                                              </CipherData>\n'
            XMLFileContents += '                                          </EncryptedData>\n'
            XMLFileContents += '                                      </passwordFromProfile>\n'
            XMLFileContents += '                                 </passwordSource>\n'
            XMLFileContents += '                             </eapLeap>\n'
            XMLFileContents += '                        </Config>\n'
            XMLFileContents += '                     </EapHostConfig>\n'
            XMLFileContents += '                  </EAPConfig>\n'
            XMLFileContents += '               </OneX>\n'
        '''
        if securityMethod == "802.1X (LEAP)":
            XMLFileContents += '                                <VendorId xmlns="http://www.microsoft.com/provisioning/EapCommon">0</VendorId>\n'
            XMLFileContents += '                                <VendorType xmlns="http://www.microsoft.com/provisioning/EapCommon">0</VendorType>\n'
            XMLFileContents += '                                <AuthorId xmlns="http://www.microsoft.com/provisioning/EapCommon">9</AuthorId>\n'
            XMLFileContents += '                             </EapMethod>\n'
            XMLFileContents += '                          <Config xmlns="http://www.microsoft.com/provisioning/EapHostConfig">\n'
            XMLFileContents += '                              <eapLeap xmlns="http://www.cisco.com/CCX">\n'
            XMLFileContents += '                                  <passwordSource>\n'
            XMLFileContents += '                                     <passwordFromProfile/>\n'
            XMLFileContents += '                                  </passwordSource>\n'
            XMLFileContents += '                              </eapLeap>\n'
            XMLFileContents += '                          </Config>\n'
            XMLFileContents += '                        </EapHostConfig>\m'
            XMLFileContents += '                     </EAPConfig>\n'
            XMLFileContents += '                  </OneX>\n'

        elif securityMethod == "802.1X (PEAP)":
            XMLFileContents += '                             <VendorId xmlns="http://www.microsoft.com/provisioning/EapCommon">0</VendorId>\n'
            XMLFileContents += '                             <VendorType xmlns="http://www.microsoft.com/provisioning/EapCommon">0</VendorType>\n'
            XMLFileContents += '                             <AuthorId xmlns="http://www.microsoft.com/provisioning/EapCommon">0</AuthorId>\n'
            XMLFileContents += '                        </EapMethod>\n'
            XMLFileContents += '                        <Config xmlns="http://www.microsoft.com/provisioning/EapHostConfig">\n'
            XMLFileContents += '                             <Eap xmlns="http://www.microsoft.com/provisioning/BaseEapConnectionPropertiesV1">\n'
            XMLFileContents += '                                  <Type>25</Type>\n'
            XMLFileContents += '                                  <EapType xmlns="http://www.microsoft.com/provisioning/MsPeapConnectionPropertiesV1">\n'
            XMLFileContents += '                                      <ServerValidation>\n'
            XMLFileContents += '                                          <DisableUserPromptForServerValidation>false</DisableUserPromptForServerValidation>\n'
            XMLFileContents += '                                          <ServerNames></ServerNames>\n'
            XMLFileContents += '                                      </ServerValidation>\n'
            XMLFileContents += '                                      <FastReconnect>true</FastReconnect>\n'
            XMLFileContents += '                                      <InnerEapOptional>false</InnerEapOptional>\n'
            XMLFileContents += '                                      <Eap xmlns="http://www.microsoft.com/provisioning/BaseEapConnectionPropertiesV1">\n'
            XMLFileContents += '                                           <Type>26</Type>\n'
            XMLFileContents += '                                           <EapType xmlns="http://www.microsoft.com/provisioning/MsChapV2ConnectionPropertiesV1">\n'
            XMLFileContents += '                                               <UseWinLogonCredentials>false</UseWinLogonCredentials>\n'
            XMLFileContents += '                                           </EapType>\n'
            XMLFileContents += '                                      </Eap>\n'
            XMLFileContents += '                                      <EnableQuarantineChecks>false</EnableQuarantineChecks>\n'
            XMLFileContents += '                                      <RequireCryptoBinding>false</RequireCryptoBinding>\n'
            XMLFileContents += '                                      <PeapExtensions>\n'
            XMLFileContents += '                                          <PerformServerValidation xmlns="http://www.microsoft.com/provisioning/MsPeapConnectionPropertiesV2">false</PerformServerValidation>\n'
            XMLFileContents += '                                          <AcceptServerName xmlns="http://www.microsoft.com/provisioning/MsPeapConnectionPropertiesV2">false</AcceptServerName>\n'
            XMLFileContents += '                                      </PeapExtensions>\n'
            XMLFileContents += '                                  </EapType>\n'
            XMLFileContents += '                               </Eap>\n'
            XMLFileContents += '                           </Config>\n'
            XMLFileContents += '                        </EapHostConfig>\n'
            XMLFileContents += '                      </EAPConfig>\n'
            XMLFileContents += '                  </OneX>\n'

        LEAPorPEAP(isPeap, EAPUserCredentialsFile, username, password, ssidText)

    elif securityMethod == "WPA2 EAP-TLS" or securityMethod == "WPA EAP-TLS":
        # EAP TLS Certificates
        XMLFileContents  = '<?xml version="1.0" encoding="US-ASCII"?>\n'
        XMLFileContents += '<WLANProfile xmlns="http://www.microsoft.com/networking/WLAN/profile/v1">\n'
        XMLFileContents += '    <name>' + ssidText + '</name>\n'
        XMLFileContents += '    <SSIDConfig>\n'
        XMLFileContents += '        <SSID>\n'
        XMLFileContents += '            <name>' + ssidText + '</name>\n'
        XMLFileContents += '        </SSID>\n'
        XMLFileContents += '        <nonBroadcast>true</nonBroadcast>\n'
        XMLFileContents += '    </SSIDConfig>\n'
        XMLFileContents += '    <connectionType>ESS</connectionType>\n'
        XMLFileContents += '    <connectionMode>auto</connectionMode>\n'
        XMLFileContents += '    <autoSwitch>false</autoSwitch>\n'
        XMLFileContents += '    <MSM>\n'
        XMLFileContents += '        <security>\n'
        XMLFileContents += '            <authEncryption>\n'
        XMLFileContents += '                <authentication>'

        #print securityMethod
        if securityMethod == "WPA2 EAP-TLS":
            #print "TODD"
            XMLFileContents += 'WPA2</authentication>\n'
        elif securityMethod == "WPA EAP-TLS":
            #print "WOOZ"
            XMLFileContents += 'WPA</authentication>\n'

        XMLFileContents += '                <encryption>' + encryption + '</encryption>\n'
        XMLFileContents += '                <useOneX>true</useOneX>\n'
        XMLFileContents += '            </authEncryption>\n'
        XMLFileContents += '            <OneX xmlns="http://www.microsoft.com/networking/OneX/v1">\n'
        XMLFileContents += '                <EAPConfig>\n'
        XMLFileContents += '                    <EapHostConfig xmlns="http://www.microsoft.com/provisioning/EapHostConfig"\n'
        XMLFileContents += '                                   xmlns:eapCommon="http://www.microsoft.com/provisioning/EapCommon"\n'
        XMLFileContents += '                                   xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapMethodConfig">\n'
        XMLFileContents += '\n'
        XMLFileContents += '                        <EapMethod>\n'
        XMLFileContents += '                            <eapCommon:Type>13</eapCommon:Type>\n'
        XMLFileContents += '                            <eapCommon:AuthorId>0</eapCommon:AuthorId>\n'
        XMLFileContents += '                        </EapMethod>\n'
        XMLFileContents += '                        <Config xmlns:baseEap="http://www.microsoft.com/provisioning/BaseEapConnectionPropertiesV1"\n'
        XMLFileContents += '                                xmlns:eapTls="http://www.microsoft.com/provisioning/EapTlsConnectionPropertiesV1">\n'
        XMLFileContents += '\n'
        XMLFileContents += '                            <baseEap:Eap>\n'
        XMLFileContents += '                                <baseEap:Type>13</baseEap:Type>\n'
        XMLFileContents += '                                <eapTls:EapType>\n'
        XMLFileContents += '                                    <eapTls:CredentialsSource>\n'
        XMLFileContents += '                                        <eapTls:CertificateStore />\n'
        XMLFileContents += '                                    </eapTls:CredentialsSource>\n'
        XMLFileContents += '                                    <eapTls:ServerValidation>\n'
        XMLFileContents += '                                        <eapTls:DisableUserPromptForServerValidation>true</eapTls:DisableUserPromptForServerValidation>\n'
        XMLFileContents += '                                        <eapTls:ServerNames />\n'
        XMLFileContents += '                                    </eapTls:ServerValidation>\n'
        XMLFileContents += '                                    <eapTls:DifferentUsername>false</eapTls:DifferentUsername>\n'
        XMLFileContents += '                                </eapTls:EapType>\n'
        XMLFileContents += '                            </baseEap:Eap>\n'
        XMLFileContents += '                        </Config>\n'
        XMLFileContents += '                    </EapHostConfig>\n'
        XMLFileContents += '                </EAPConfig>\n'
        XMLFileContents += '            </OneX>\n'

    #print XMLFileContents
    XMLFileContents += endText

    #print(XMLFileContents)
    
    #print wirelessProfileXMLFile
    # DELETE EXISTING PROFILE FILE
    if os.path.isfile(wirelessProfileXMLFile):
        try:
            #print "worked"
            os.remove(wirelessProfileXMLFile)
        except:
            #print "failed"
            pass
    
    # CREATE AND WRITE EXISTING PROFILE FILE
    try:
        f = open(wirelessProfileXMLFile,  'w')
        f.write(XMLFileContents)
        f.close()
        #print "yes go"
        return True
    except:
        #print "no go"
        return False
    
if __name__ == "__main__":
    p = OptionParser()
    # Now add some CLI options
    ##########################
    p.add_option("-X", "--wirelessProfileXMLFilename",
                 dest="profileXMLFilename",
                 help="Wireless Profile XML Filename typcially WirelessProfile")
    p.add_option("-U", "--eapUserCredentialsXMLFilename",
                 dest="eapXMLFilename",
                 help="EAP UserPass XML Filename typically EAPXMLFile")
    p.add_option("-S", "--ssid",
                 dest="ssid",
                 help="The SSID")
    p.add_option("-E", "--encryption",
                 dest="encryption",
                 help="The encryption method")
    p.add_option("-M", "--securityMethod",
                 dest="securityMethod",
                 help="The security method")
    p.add_option("-K", "--keyType",
                 dest="keyType",
                 help="The keyType AES or TKIP")
    p.add_option("-P", "--passphrase",
                 dest="passphrase",
                 help="The passphrase")
    p.add_option("-D", "--defaultKey",
                 dest="defaultKey",
                 help="The dfault Key number")
    p.add_option("-1", "--Key1",
                 dest="key1",
                 help="wep key 1")
    p.add_option("-2", "--Key2",
                 dest="key2",
                 help="wep key 2")
    p.add_option("-3", "--Key3",
                 dest="key3",
                 help="wep key 3")
    p.add_option("-4", "--Key4",
                 dest="key4",
                 help="wep key 4")
    p.add_option("-u", "--username",
                 dest="username",
                 help="username of eap")
    p.add_option("-p", "--password",
                 dest="password",
                 help="password of eap")
    p.add_option("-v", "--verbose",
                 dest="verbose", default=None, action="store_true",
                 help="Print detailed status")
    p.add_option("-q", "--quiet",
                 dest="verbose", default=None, action="store_false",
                 help="No status printing (default)")

    # Parse User's CLI args
    #######################
    options, args = p.parse_args()
    #print options.ssid
    #print options
    #print "VVVVVVV"
    #print args

    writeXML(options.profileXMLFilename, options.eapXMLFilename, options.ssid, options.encryption, options.securityMethod, options.keyType, options.passphrase, options.defaultKey, options.key1, options.key2, options.key3, options.key4, options.username, options.password)
    sys.exit(0)
