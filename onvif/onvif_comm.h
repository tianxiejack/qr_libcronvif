#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "soapH.h"
#include "wsaapi.h"

#ifndef __ONVIF_COMM_H__
#define __ONVIF_COMM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DIM
#define DIM(array)  (sizeof(array) / sizeof(array[0]))
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#define SOAP_ASSERT     assert
#define SOAP_DBGLOG     printf
#define SOAP_DBGERR     printf

#define USERNAME        "onvif"                                                 // ��֤��Ϣ���û������룩
#define PASSWORD        "12345qwert"//"12345qazwsx"

#define SOAP_TO         "urn:schemas-xmlsoap-org:ws:2005:04:discovery"
#define SOAP_ACTION     "http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe"

#define SOAP_MCAST_ADDR "soap.udp://239.255.255.250:3702"                       // onvif�涨���鲥��ַ

#define SOAP_ITEM       ""                                                      // Ѱ�ҵ��豸��Χ
#define SOAP_TYPES      "dn:NetworkVideoTransmitter"                            // Ѱ�ҵ��豸����

#define SOAP_SOCK_TIMEOUT    (1)                                               // socket��ʱʱ�䣨�����룩

#define ONVIF_ADDRESS_SIZE   (128)                                              // URI��ַ����
#define ONVIF_TOKEN_SIZE     (65)                                               // token����

/* ��Ƶ������������Ϣ */
struct tagVideoEncoderConfiguration
{
    char token[ONVIF_TOKEN_SIZE];                                               // Ψһ��ʶ����Ƶ�������������ַ�
    int Width;                                                                  // �ֱ���
    int Height;
};

/* �豸������Ϣ */
struct tagProfile {
    char token[ONVIF_TOKEN_SIZE];                                               // Ψһ��ʶ�豸�����ļ��������ַ�

    struct tagVideoEncoderConfiguration venc;                                   // ��Ƶ������������Ϣ
};

/* �豸������Ϣ */
struct tagCapabilities {
    char MediaXAddr[ONVIF_ADDRESS_SIZE];                                        // ý������ַ
    char EventXAddr[ONVIF_ADDRESS_SIZE];                                        // �¼������ַ
                                                                                // �����������ַ�Ͳ��г���
};

#define SOAP_CHECK_ERROR(result, soap, str) \
    do { \
        if (SOAP_OK != (result) || SOAP_OK != (soap)->error) { \
            soap_perror((soap), (str)); \
            if (SOAP_OK == (result)) { \
                (result) = (soap)->error; \
            } \
            goto EXIT; \
        } \
    } while (0)


void            soap_perror(struct soap *soap, const char *str);
void *          ONVIF_soap_malloc(struct soap *soap, unsigned int n);
struct soap *   ONVIF_soap_new(int timeout);
void            ONVIF_soap_delete(struct soap *soap);

int             ONVIF_SetAuthInfo(struct soap *soap, const char *username, const char *password);
void            ONVIF_init_header(struct soap *soap);
void            ONVIF_init_ProbeType(struct soap *soap, struct wsdd__ProbeType *probe);
void            ONVIF_DetectDevice(void (*cb)(char *DeviceXAddr,const char* username,const char* password),const char* name,const char* pswd);

int             ONVIF_GetCapabilities2(const char *DeviceXAddr, struct tagCapabilities *capa,const char* username,const char* password);
int             ONVIF_GetProfiles(const char *MediaXAddr, struct tagProfile **profiles,const char* username,const char* password);
int             make_uri_withauth(char *src_uri, char *username, char *password, char *dest_uri, unsigned int size_dest_uri);

int 			ONVIF_GetDeviceInformation(const char *DeviceXAddr,const char* username,const char* password);
int 			ONVIF_GetCapabilities(const char *DeviceXAddr,const char* username,const char* password);

#ifdef __cplusplus
}
#endif

#endif
