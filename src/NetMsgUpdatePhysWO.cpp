#include "NetMsgCreateWO.h"
#ifdef AFTR_CONFIG_USE_BOOST

#include <iostream>
#include <sstream>
#include <string>
#include "AftrManagers.h"
#include "Vector.h"
#include "WO.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "NetMsgCreateWOReply.h"
#include "NetMsgUpdatePhysWO.h"
#include "GLViewNat22_Module7.h"

using namespace Aftr;

NetMsgMacroDefinition(NetMsgUpdatePhysWO);

NetMsgUpdatePhysWO::NetMsgUpdatePhysWO()
{
	this->xPos = 0;
	this->yPos = 0;
	this->zPos = 0;

	this->mat1_1 = 0;
	this->mat1_2 = 0;
	this->mat1_3 = 0;
	this->mat2_1 = 0;
	this->mat2_2 = 0;
	this->mat2_3 = 0;
	this->mat3_1 = 0;
	this->mat3_2 = 0;
	this->mat3_3 = 0;

}

NetMsgUpdatePhysWO::~NetMsgUpdatePhysWO()
{
}

bool NetMsgUpdatePhysWO::toStream(NetMessengerStreamBuffer& os) const
{

	os << this->xPos;
	os << this->yPos;
	os << this->zPos;

	os << this->mat1_1;
	os << this->mat1_2;
	os << this->mat1_3;
	os << this->mat2_1;
	os << this->mat2_2;
	os << this->mat2_3;
	os << this->mat3_1;
	os << this->mat3_2;
	os << this->mat3_3;

	return true;
}

bool NetMsgUpdatePhysWO::fromStream(NetMessengerStreamBuffer& is)
{
	is >> this->xPos;
	is >> this->yPos;
	is >> this->zPos;

	is >> this->mat1_1;
	is >> this->mat1_2;
	is >> this->mat1_3;
	is >> this->mat2_1;
	is >> this->mat2_2;
	is >> this->mat2_3;
	is >> this->mat3_1;
	is >> this->mat3_2;
	is >> this->mat3_3;

	return true;
}

void NetMsgUpdatePhysWO::onMessageArrived()
{
	if (!ManagerGLView::getGLViewT<GLViewNat22_Module7>()->isClient) return;
	std::cout << "Message has arrived... " << this->toString() << "\n";

	WO* wo = ManagerGLView::getGLViewT<GLViewNat22_Module7>()->ClientObject;
	std::cout << "Payload\n";
	std::cout << xPos << ",";
	std::cout << yPos << ",";
	std::cout << zPos << ".\n";
	wo->setPosition(Vector(xPos, yPos, zPos));

	float temp[16]{
		mat1_1,
		mat1_2,
		mat1_3,
		0,
		mat2_1,
		mat2_2,
		mat2_3,
		0,
		mat3_1,
		mat3_2,
		mat3_3,
		0,
		0,
		0,
		0,
		1
	};

	wo->setDisplayMatrix(Mat4(temp));
	wo->setLabel("Created by IPC Net Msg");
	//ManagerGLView::getGLView()->getWorldContainer()->push_back(wo);

	auto msg = std::make_shared<NetMsgCreateWOReply>();
	msg->woID = wo->getID();
	this->sendReply(msg);
}

std::string NetMsgUpdatePhysWO::toString() const
{
	std::stringstream ss;

	ss << NetMsg::toString();
	//ss << "   Payload: " << Vector( xPos, yPos, zPos ).toString() << "...\n";
	ss << "Payload\n";
	ss << xPos << ",";
	ss << yPos << ",";
	ss << zPos << ".\n";

	ss << mat1_1 << " " << mat1_2 << " " << mat1_3 << " " << 0 << "\n" << mat2_1 << " " << mat2_2 << " " << mat2_3 << " " << 0 << "\n" << mat3_1 << " " << mat3_2 << " " << mat3_3 << " " << 0 << "\n" << 0 << " " << 0 << " " << 0 << " " << 1 << '\n';


	return ss.str();
}

#endif