class XMLFormat;

class StringType;

class OnlineHelp
{
	protected:

	OnlineHelp();

	public:

	static XMLFormat lookupHelp(StringType helpScreenId);

	static XMLFormat lookupHelp(StringType helpScreenId, StringType fieldId);

	static OnlineHelp * getInstance();

	private:

	/** @link
	 * @shapeType PatternLink
	 * @pattern <{Singleton}>
	 * @supplierRole Singleton factory */
/*# OnlineHelp _onlineHelp; */
	static OnlineHelp * instance;
};




