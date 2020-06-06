******************************************************************************
*                                                                                                                  *
*  Readme-NitroWiFi-1_1-060616.txt                                                                    *
*                                                                                                                  *
*   NITRO-SDK Wi-Fi Library 1.1                                                                         *
*                                                                                                                  *
*  June 16, 2006                                                                                              *
*                                                                                                                  *
******************************************************************************

Introduction

- The NITRO-SDK Wi-Fi Library is an all-inclusive term for the socket libraries used in the development of game software for the Nintendo DS.
By using this library, it is possible to access the Internet using Nintendo DS Wireless Networking (Infrastructure Mode).  
- For further details, please see the Function Reference.
    
- The development environment supported by this package is as follows:
  
* NITRO-SDK 3.0 or later (recommended for versions 3.1 and later)
* CodeWarrior for NINTENDO DS version 1.2 and later (supporting version 2.0)

--

[Update Details]

- The details of the update from NITRO-SDK Wi-Fi Library 1.0 are as follows:

* (CPS) Added a timeout process to the SSL key exchange
  
When enabling SSL and attempting a TCP connection via the SOC_Connect function, the encryption keys are exchanged internally. Up until now, there was no timeout in this key exchange process, but it is now set to time out. This change will resolve the phenomenon where, when a user disables connection to a Wi-Fi USB connector during DWC authentication, the authentication process of the disabled DS authentication process would never end.
  
* (CPS) Made changes to the send timing of the TCP window update
  
There were cases where the window update would not be sent out when the receive buffer went from a state where it had no space to one where it had space. We have made changes so that the window update will now be sent out.
  
* (CPS) Improved the exclusion control
  
When sockets were repeatedly generated and discarded at a high frequency, there were cases where memory was accessed illegally. This problem has been corrected. There were also incidents of stoppage during the TCP receive process. These have also been corrected.

--

Notes

- The NITRO-SDK Wi-Fi library employs cryptographic features defined under the laws and regulations such as Foreign Exchange and Trade Law, Export Trade Control Ordinance, and Foreign Exchange Order of Japan. To export the NITRO-SDK Crypto library from Japan, you must apply for an export license to the Ministry of Economy, Trade, and Industry of Japan and obtain the license in advance. Use proper care and caution when using this library. The export regulations and laws of each nation involved must be observed when exporting this library package.

--

End
