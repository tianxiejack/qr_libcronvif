/*
 * onvifHandle.cpp
 *
 *  Created on: 2019年8月20日
 *      Author: alex
 */

#include "onvifHandle.hpp"

#include "onvif_dump.h"
#include "onvif_comm.h"
#include "wsseapi.h"
#include "stdsoap2.h"

#include <math.h>

IOnvif* IOnvif::getinstance(const char* ip,const char* username,const char* password)
{
	IOnvif* ponvif = new COnvif(ip,username,password);
	return ponvif;
}

COnvif::COnvif(const char* ip,const char* username,const char* password)
{
	memset(m_profile,'\0',sizeof(m_profile));	
	memset(m_ip,'\0',sizeof(m_ip));
	memset(m_endpoint, '\0', sizeof(m_endpoint));
	memset(m_PTZendpoint, '\0', sizeof(m_PTZendpoint));
	memset(m_username,'\0',sizeof(m_username));
	memset(m_password,'\0',sizeof(m_password));
	init(ip,username,password);
}

COnvif::~COnvif()
{
}

void COnvif::init(const char* ip,const char* username,const char* password)
{
	sprintf(m_ip,ip);
	sprintf(m_endpoint, "http://%s/onvif/device_service", m_ip);  
	sprintf(m_PTZendpoint, "http://%s/onvif/PTZ", m_ip);
	sprintf(m_username,username);
	sprintf(m_password,password);
	
	getProfileToken();
	return;
}

void COnvif::detectDevice()
{
	ONVIF_DetectDevice(cb_discovery,m_username,m_password);
	return;
}


void COnvif::cb_discovery(char *DeviceXAddr,const char* username,const char* password)
{
	ONVIF_GetDeviceInformation(DeviceXAddr,username,password);
	ONVIF_GetCapabilities(DeviceXAddr,username,password);
	return;
}


void COnvif::getProfileToken()
{
	struct soap *soap = NULL;
	SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

	char Mediaddr[256]="";
	
	struct _tds__GetCapabilities			req;
	struct _tds__GetCapabilitiesResponse	rep;
	struct _trt__GetProfiles				getProfiles;
	struct _trt__GetProfilesResponse		response;	
			
	req.Category = (enum tt__CapabilityCategory *)soap_malloc(soap, sizeof(int));
	req.__sizeCategory = 1;
	*(req.Category) = (enum tt__CapabilityCategory)0;
	   	
	soap_call___tds__GetCapabilities(soap, m_endpoint, NULL, &req, &rep);
	if (soap->error)  
	{  
		printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__, 
											soap->error, *soap_faultcode(soap), 
											*soap_faultstring(soap));	 
	} 
	else
	{
		printf("get capability success\n");
		//printf("Dev_XAddr====%s\n",rep.Capabilities->Device->XAddr);
		printf("Med_XAddr====%s\n",rep.Capabilities->Media->XAddr);
		//printf("PTZ_XAddr====%s\n",rep.Capabilities->PTZ->XAddr);
		strcpy(Mediaddr,rep.Capabilities->Media->XAddr);
	}	
	printf("\n");
	
	soap_wsse_add_UsernameTokenDigest(soap, NULL, m_username, m_password);
	if(soap_call___trt__GetProfiles(soap,Mediaddr,NULL,&getProfiles,&response)==SOAP_OK)
	{
		strcpy(m_profile, response.Profiles[0].token);
		printf("get profile succeed \n");		
		printf("profile====%s\n",m_profile);	
	}
	else
	{
		printf("get profile failed \n");
		printf("[%s][%d]--->>> soap result: %d, %s, %s\n", __func__, __LINE__, 
											soap->error, *soap_faultcode(soap), 
											*soap_faultstring(soap));  
	}

	ONVIF_soap_delete(soap); 
	return;	
}



int COnvif::getPtzStatus(float& p,float& t,float& z)
{
	struct soap *soap = NULL;
	SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

	struct _tptz__GetStatus statusReq;
	struct _tptz__GetStatusResponse statusRep;
	memset(&statusReq, 0x0, sizeof(statusReq));
	memset(&statusRep, 0x0, sizeof(statusRep));

	statusReq.ProfileToken = m_profile;
	soap_wsse_add_UsernameTokenDigest(soap, NULL, m_username, m_password);

	int result;
	result = soap_call___tptz__GetStatus(soap, m_PTZendpoint, NULL, &statusReq, &statusRep);			

	SOAP_CHECK_ERROR(result, soap, "GetPTZstatus");

	if (NULL != statusRep.PTZStatus)
	{
        if (NULL != statusRep.PTZStatus->Position) 
		{
			p = statusRep.PTZStatus->Position->PanTilt->x;
			t = statusRep.PTZStatus->Position->PanTilt->y;
			z = statusRep.PTZStatus->Position->Zoom->x;
		}
	}
	
EXIT:
	ONVIF_soap_delete(soap);
    return result;
}


int COnvif::stop()
{
	struct soap *soap = NULL;
	SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));
	
	struct _tptz__Stop ptz_req;
	struct _tptz__StopResponse ptz_resp;
	memset(&ptz_req, 0x0, sizeof(ptz_req));
	memset(&ptz_resp, 0x0, sizeof(ptz_resp));
			
	ptz_req.ProfileToken = m_profile;
	ptz_req.PanTilt = soap_new_xsd__boolean(soap,-1);
	ptz_req.Zoom = soap_new_xsd__boolean(soap,-1);
	
	*ptz_req.PanTilt = xsd__boolean__true_;
	*ptz_req.Zoom = xsd__boolean__true_;
	
	soap_wsse_add_UsernameTokenDigest(soap, NULL, m_username, m_password);
	int result = soap_call___tptz__Stop(soap, m_PTZendpoint, NULL, &ptz_req, &ptz_resp);
	SOAP_CHECK_ERROR(result, soap, "ptzStop");
	
EXIT:	 
	ONVIF_soap_delete(soap); 
	return result;
}


int COnvif::continuesMove(float p,float t,float z)
{
	struct soap *soap = NULL;
	SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

	struct _tptz__ContinuousMove ptz_req;
	struct _tptz__ContinuousMoveResponse ptz_resp;
	memset(&ptz_req, 0x0, sizeof(ptz_req));
	memset(&ptz_resp, 0x0, sizeof(ptz_resp));
			
	ptz_req.ProfileToken = m_profile;
	
	ptz_req.Velocity = soap_new_tt__PTZSpeed(soap,-1);
	ptz_req.Velocity->PanTilt = soap_new_tt__Vector2D(soap, -1);
	ptz_req.Velocity->Zoom = soap_new_tt__Vector1D(soap,-1);
	
	if((fabs(p)-1.0) < m_FLOATZERO)
		ptz_req.Velocity->PanTilt->x = p;

	if((fabs(t)-1.0) < m_FLOATZERO)
		ptz_req.Velocity->PanTilt->y = t;

	if((fabs(z)-1.0) < m_FLOATZERO)
		ptz_req.Velocity->Zoom->x = z;
		
	soap_wsse_add_UsernameTokenDigest(soap, NULL, m_username, m_password);

	int result = soap_call___tptz__ContinuousMove(soap, m_PTZendpoint, NULL, &ptz_req, &ptz_resp);
	SOAP_CHECK_ERROR(result, soap, "continusMove");

EXIT:
	ONVIF_soap_delete(soap); 
    return result;	
}


int COnvif::relativeMove(float p,float t,float z)
{
	struct soap *soap = NULL;
	SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

	char Mediaddr[256]="";

	struct _tptz__RelativeMove ptz_req;
	struct _tptz__RelativeMoveResponse ptz_resp;
	memset(&ptz_req, 0x0, sizeof(ptz_req));
	memset(&ptz_resp, 0x0, sizeof(ptz_resp));
	
	ptz_req.ProfileToken = m_profile;
	ptz_req.Translation = soap_new_tt__PTZVector(soap, -1);
	ptz_req.Translation->PanTilt =  soap_new_tt__Vector2D(soap, -1);
	ptz_req.Translation->Zoom = soap_new_tt__Vector1D(soap,-1);

	if((fabs(p)-1.0) < m_FLOATZERO)
		ptz_req.Translation->PanTilt->x = p;

	if((fabs(t)-1.0) < m_FLOATZERO)
		ptz_req.Translation->PanTilt->y = t;
	
	if((fabs(z)-1.0) < m_FLOATZERO)
		ptz_req.Translation->Zoom->x = z;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, m_username, m_password);
	int result = soap_call___tptz__RelativeMove(soap, m_PTZendpoint, NULL, &ptz_req, &ptz_resp);
	SOAP_CHECK_ERROR(result, soap, "relativeMove");
	
EXIT:
	ONVIF_soap_delete(soap); 
    return result;
}


int COnvif::absoluteMove(float p,float t,float z,float pspeed,float tspeed,float zspeed)
{
	struct soap *soap = NULL;
	SOAP_ASSERT(NULL != (soap = ONVIF_soap_new(SOAP_SOCK_TIMEOUT)));

	char Mediaddr[256]="";

	struct _tptz__AbsoluteMove           absoluteMove;
	struct _tptz__AbsoluteMoveResponse   absoluteMoveResponse;
	memset(&absoluteMove, 0x0, sizeof(absoluteMove));
	memset(&absoluteMoveResponse, 0x0, sizeof(absoluteMoveResponse));

	absoluteMove.ProfileToken = m_profile;

	absoluteMove.Position = soap_new_tt__PTZVector(soap, -1);
	absoluteMove.Position->PanTilt = soap_new_tt__Vector2D(soap, -1);
	absoluteMove.Speed = soap_new_tt__PTZSpeed(soap, -1);
	absoluteMove.Speed->PanTilt = soap_new_tt__Vector2D(soap, -1);

	absoluteMove.Position->Zoom = soap_new_tt__Vector1D(soap, -1);
	absoluteMove.Speed->Zoom = soap_new_tt__Vector1D(soap, -1);

	if((fabs(p)-1.0) < m_FLOATZERO)
		absoluteMove.Position->PanTilt->x = p;
	
	if((fabs(pspeed)-1.0) < m_FLOATZERO)
		absoluteMove.Speed->PanTilt->x = pspeed;
	
	if((fabs(t)-1.0) < m_FLOATZERO)
		absoluteMove.Position->PanTilt->y = t;

	if((fabs(tspeed)-1.0) < m_FLOATZERO)
		absoluteMove.Speed->PanTilt->y = tspeed;

	if((fabs(z)-1.0) < m_FLOATZERO)
		absoluteMove.Position->Zoom->x = z;

	if((fabs(zspeed)-1.0) < m_FLOATZERO)
		absoluteMove.Speed->Zoom->x = zspeed;

	soap_wsse_add_UsernameTokenDigest(soap, NULL, m_username, m_password);
	int result = soap_call___tptz__AbsoluteMove(soap, m_PTZendpoint, NULL, &absoluteMove, 
	                                        &absoluteMoveResponse);		
	
	SOAP_CHECK_ERROR(result, soap, "absoluteMove");
EXIT:
	ONVIF_soap_delete(soap); 
    return result;
}

