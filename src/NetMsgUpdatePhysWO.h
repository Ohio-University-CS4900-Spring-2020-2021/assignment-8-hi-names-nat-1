#pragma once

#include "NetMsg.h"
#include "WO.h"
#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr
{

	class NetMsgUpdatePhysWO : public NetMsg
	{
	public:
		NetMsgMacroDeclaration(NetMsgUpdatePhysWO);

		NetMsgUpdatePhysWO();
		virtual ~NetMsgUpdatePhysWO();
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

		WO* wo;

		float xPos;
		float yPos;
		float zPos;

		float mat1_1, mat1_2, mat1_3, mat2_1, mat2_2, mat2_3, mat3_1, mat3_2, mat3_3;

		//WO* wo;

	protected:

	};

} //namespace Aftr

#endif