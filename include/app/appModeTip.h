
class appModeTip : public abstractAppMode {
// parent class for modes with a tip

	public:
	
	abstractTip* tip = NULL;

	appModeTip(){};
	appModeTip( graphInterface* graphics_, int numOfMoleculeInstances_, abstractTip* tip_, flagList *flags );
	~appModeTip();

};

// ================= appModeTip procedures ================= 

appModeTip::~appModeTip(){
// destructor

	delete tip;

}

appModeTip::appModeTip( graphInterface* graphics_, int numOfMoleculeInstances_, abstractTip* tip_, flagList *flags )
// constructor
: tip( tip_ ), abstractAppMode( graphics_, numOfMoleculeInstances_, flags )
{
}
