/*
 * onvifHandle.hpp
 *
 *  Created on: 2019年8月20日
 *      Author: alex
 */

#ifndef ONVIFHANDLE_HPP_
#define ONVIFHANDLE_HPP_


#include "onvifInterface.hpp"

class COnvif : public IOnvif
{
public:
	COnvif(const char* ip,const char* username,const char* password);
	virtual ~COnvif();
	
	void detectDevice();
	int stop();
	int continuesMove(float p,float t,float z);
	int relativeMove(float p,float t,float z);
	int absoluteMove(float p,float t,float z,float pspeed,float tspeed,float zspeed);
	int getPtzStatus(float& p,float& t,float& z);


private:
	void init(const char* ip,const char* username,const char* password);
	void getProfileToken();
	
	

private:	
	static void cb_discovery(char *DeviceXAddr,const char* username,const char* password);
	char m_profile[256];
	char m_ip[256];
	char m_endpoint[255];
	char m_PTZendpoint[255];
	const float m_FLOATZERO = 0.000001;
	char m_username[256];
	char m_password[256];
};

#endif /* ONVIFHANDLE_HPP_ */
