/*
 * onvifInterface.hpp
 *
 *  Created on: 2019年8月20日
 *      Author: alex
 */

#ifndef ONVIFINTERFACE_HPP_
#define ONVIFINTERFACE_HPP_

class IOnvif
{
public:
	static IOnvif* getinstance();
	virtual void detectDevice()=0;
	virtual int continuesMove(float p,float t,float z)=0;
	virtual int relativeMove(float p,float t,float z)=0;
	virtual int absoluteMove(float p,float t,float z,float pspeed,float tspeed,float zspeed)=0;
	virtual int getPtzStatus(float& p,float& t,float& z)=0;
};

#endif /* ONVIFINTERFACE_HPP_ */
