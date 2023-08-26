#include "Pattern.h"
#include "Node.h"
namespace yfpeg {

	//And
	Pattern& Pattern::operator*(Pattern& rvalue) {
		AndNode* newNode = new AndNode();
		newNode->addChild(pNode);
		newNode->addChild(rvalue.pNode);
		return *new Pattern(newNode);
	}
	//Or
	Pattern& Pattern::operator+(Pattern& rvalue) {
		OrNode* newNode = new OrNode();
		newNode->addChild(pNode);
		newNode->addChild(rvalue.pNode);
		return *new Pattern(newNode);
	}
	//Not
	Pattern& Pattern::operator-(Pattern& rvalue) {
		AndNode* newNode = new AndNode();
		NotNode* notNode = new NotNode(rvalue.pNode);
		newNode->addChild(notNode);
		newNode->addChild(this->pNode);
		return *new Pattern(newNode);
	}
	//Assign
	Pattern& Pattern::operator=(Pattern& rvalue) {
		ReferenceNode* refNode = CAST_TO(this->pNode, ReferenceNode);
		if (refNode)
		{
			refNode->setInnerNode(rvalue.pNode);
		}
		else {
			this->pNode = rvalue.pNode;
		}
		return *this;
	};
	//Capture
	Pattern& Pattern::operator<<(string tag) {
		return *new Pattern(new CaptureNode(this->pNode,tag));
	};
	Pattern& Pattern::operator>>(FILTER_TYPE filterType) {
		return *new Pattern(new FilterNode(this->pNode,filterType));
	};
	//atLeast or atMost
	Pattern& Pattern::operator^(int times) {
		RepeatNode* repeat = nullptr;
		if (times >= 0) {
			 repeat = new RepeatNode(this->pNode,times,true);
		}
		else {
			 repeat = new RepeatNode(this->pNode,-times,false);
		}
		return *new Pattern(repeat);
	}
	//Callback
	Pattern& Pattern::operator/(Pattern& rvalue) {
		return *this;
	}
}
