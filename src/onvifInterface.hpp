#ifndef ONVIFINTERFACE_HPP_
#define ONVIFINTERFACE_HPP_

class IOnvif
{
public:
	static IOnvif* getinstance(const char* ip,const char* username,const char* password);
	virtual void detectDevice()=0;
	virtual int stop()=0;
	virtual int continuesMove(float p,float t,float z)=0;
	virtual int relativeMove(float p,float t,float z)=0;
	virtual int absoluteMove(float p,float t,float z,float pspeed,float tspeed,float zspeed)=0;
	virtual int getPtzStatus(float& p,float& t,float& z)=0;
};

#endif /* ONVIFINTERFACE_HPP_ */
