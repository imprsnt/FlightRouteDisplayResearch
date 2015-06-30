#include "UtilityClasses.h"
#include <QFile>
#include <QTextStream>
#include <QtCore>
#include <QDebug>
#include <QRegExp>
#include <QtXml/QtXml>
#include <QFileDialog>
#include <QErrorMessage>


AirportList::AirportList(){

    airPortCodes.push_back("Select Airport");
    airPortCodes.push_back("ALB ALBANY, NY");
    airPortCodes.push_back("ABQ ALBUQUERQUE,NM");
    airPortCodes.push_back("ESF	ALEXANDRIA, LA");
    airPortCodes.push_back("ABE	ALLENTOWN, PA");
    airPortCodes.push_back("AMA	AMARILLO, TX");
    airPortCodes.push_back("ANC	ANCHORAGE, AK");
    airPortCodes.push_back("ATL	ATLANTA, GA");
    airPortCodes.push_back("AUS	AUSTIN, TX");
    airPortCodes.push_back("BFL	BAKERSFIELD, CA");
    airPortCodes.push_back("BWI	BALTIMORE, MD");
    airPortCodes.push_back("BGR	BANGOR, ME");
    airPortCodes.push_back("BTR	BATON ROUGE, LA");
    airPortCodes.push_back("MBS	BAY CITY, MI");
    airPortCodes.push_back("BPT	BEAUMONT/PORT ARTHUR, TX");
    airPortCodes.push_back("BIL	BILLINGS, MT");
    airPortCodes.push_back("BHM	BIRMINGHAM, AL");
    airPortCodes.push_back("BMI	BLOOMINGTON, IL");
    airPortCodes.push_back("BOI	BOISE, ID");
    airPortCodes.push_back("BOS	BOSTON, MA");
    airPortCodes.push_back("BUF	BUFFALO, NY");
    airPortCodes.push_back("BUR	BURBANK, CA");
    airPortCodes.push_back("BTM	BUTTE, MT");
    airPortCodes.push_back("CID	CEDAR RAPIDS, IA");
    airPortCodes.push_back("CMI	CHAMPAIGN/URBANA, IL");
    airPortCodes.push_back("CHS	CHARLESTON, SC");
    airPortCodes.push_back("CLT	CHARLOTTE, NC");
    airPortCodes.push_back("CHA	CHATTANOOGA, TN");
    airPortCodes.push_back("MDW	CHICAGO, IL/MIDWAY");
    airPortCodes.push_back("ORD	CHICAGO, IL/O'HARE");
    airPortCodes.push_back("CVG	CINCINNATI, OH");
    airPortCodes.push_back("CLE	CLEVELAND, OH");
    airPortCodes.push_back("COS	COLORADO SPRINGS, CO");
    airPortCodes.push_back("CAE	COLUMBIA, SC");
    airPortCodes.push_back("CMH	COLUMBUS,OH");
    airPortCodes.push_back("CRP	CORPUS CHRISTI, TX");
    airPortCodes.push_back("DFW	DALLAS/FT.WORTH, TX");
    airPortCodes.push_back("DAY	DAYTON, OH");
    airPortCodes.push_back("DEN	DENVER, CO");
    airPortCodes.push_back("DSM	DES MOINES, IA");
    airPortCodes.push_back("DET	DETROIT, MI./CITY");
    airPortCodes.push_back("DTW	DETROIT, MI./METRO");
    airPortCodes.push_back("DBQ	DUBUQUE, IA");
    airPortCodes.push_back("DRO	DURANGO, CO");
    airPortCodes.push_back("DUT	DUTCH HARBOR, AK");
    airPortCodes.push_back("ELP	EL PASO, TX");
    airPortCodes.push_back("EUG	EUGENE, OR");
    airPortCodes.push_back("EVV	EVANSVILLE, IN");
    airPortCodes.push_back("FAI	FAIRBANKS, AK");
    airPortCodes.push_back("FYV	FAYETTEVILLE, AR");
    airPortCodes.push_back("FLO	FLORENCE, SC");
    airPortCodes.push_back("FAT	FRESNO, CA");
    airPortCodes.push_back("FLL	FT. LAUDERDALE, FL");
    airPortCodes.push_back("RSW	FT. MYERS, FL");
    airPortCodes.push_back("FSM	FT. SMITH, AR");
    airPortCodes.push_back("VPS	FT. WALTON BEACH, FL");
    airPortCodes.push_back("FWA	FT. WAYNE, IN");
    airPortCodes.push_back("GRR	GRAND RAPIDS, MI");
    airPortCodes.push_back("GRB	GREEN BAY, WI");
    airPortCodes.push_back("GSP	GREENVILLE/SPARTANBURG, SC");
    airPortCodes.push_back("GSO	GREENSBORO, NC");
    airPortCodes.push_back("GUC	GUNNISON, CO");
    airPortCodes.push_back("HRL	HARLINGEN, TX");
    airPortCodes.push_back("MDT	HARRISBURG, PA");
    airPortCodes.push_back("BDL	HARTFORD, CT");
    airPortCodes.push_back("HNL	HONOLULU, HI");
    airPortCodes.push_back("HOU	HOUSTON, TX/HOBBY");
    airPortCodes.push_back("IAH	HOUSTON,TX/INTERCONTINENTAL");
    airPortCodes.push_back("HSV	HUNTSVILLE, AL");
    airPortCodes.push_back("IND	INDIANAPOLIS, IN");
    airPortCodes.push_back("ISP	 ISLIP, NY");
    airPortCodes.push_back("JAN	JACKSON, MS");
    airPortCodes.push_back("JAC	JACKSON HOLE, WY");
    airPortCodes.push_back("JAX	JACKSONVILLE, FL");
    airPortCodes.push_back("AZO	KALAMAZOO, MI");
    airPortCodes.push_back("MCI	KANSAS CITY, MO");
    airPortCodes.push_back("EEN	KEENE, NH");
    airPortCodes.push_back("EYW	KEY WEST, FL");
    airPortCodes.push_back("ILE	 KILLEEN, TX");
    airPortCodes.push_back("TYS	KNOXVILLE, TN");
    airPortCodes.push_back("LSE	LA CROSSE, WI");
    airPortCodes.push_back("LFT	LAFAYETTE, LA");
    airPortCodes.push_back("LCH	LAKE CHARLES, LA");
    airPortCodes.push_back("LAN	LANSING, MI");
    airPortCodes.push_back("LRD	LAREDO, TX");
    airPortCodes.push_back("LAS	LAS VEGAS, NV");
    airPortCodes.push_back("LAW	LAWTON, OK");
    airPortCodes.push_back("LEX	LEXINGTON, KY");
    airPortCodes.push_back("LNK	LINCOLN, NE");
    airPortCodes.push_back("LIT	LITTLE ROCK, AR");
    airPortCodes.push_back("GGG	LONGVIEW, TX");
    airPortCodes.push_back("LAX	LOS ANGELES, CA");
    airPortCodes.push_back("SDF	LOUISVILLE, KY");
    airPortCodes.push_back("LBB	LUBBOCK, TX");
    airPortCodes.push_back("MSN	MADISON, WI");
    airPortCodes.push_back("MHT	MANCHESTER, NH");
    airPortCodes.push_back("MQT	MARQUETTE, MI");
    airPortCodes.push_back("OGG	MAUI, HI");
    airPortCodes.push_back("MFE	MCALLEN, TX");
    airPortCodes.push_back("MFR	MEDFORD, OR");
    airPortCodes.push_back("MLB	MELBOURNE, FL");
    airPortCodes.push_back("MEM	MEMPHIS, TN");
    airPortCodes.push_back("MIA	MIAMI, FL");
    airPortCodes.push_back("MKE	MILWAUKEE, WI");
    airPortCodes.push_back("MSP	MINNEAPOLIS, MN");
    airPortCodes.push_back("MOB	MOBILE, AL");
    airPortCodes.push_back("MLI	MOLINE, IL");
    airPortCodes.push_back("MRY	MONTEREY, CA");
    airPortCodes.push_back("MGM	MONTGOMERY, AL");
    airPortCodes.push_back("MKG	MUSKEGON, MI");
    airPortCodes.push_back("MYR	MYRTLE BEACH, SC");
    airPortCodes.push_back("BNA	NASHVILLE, TN");
    airPortCodes.push_back("HVN	NEW HAVEN, CT");
    airPortCodes.push_back("MSY	NEW ORLEANS, LA");
    airPortCodes.push_back("JFK	NEW YORK, NY/KENNEDY");
    airPortCodes.push_back("LGA	NEW YORK, NY/LA GUARDIA");
    airPortCodes.push_back("EWR	NEWARK, NJ");
    airPortCodes.push_back("SWF	NEWBURGH/STEWART FIELD, NY");
    airPortCodes.push_back("ORF	NORFOLK, VA");
    airPortCodes.push_back("OAK	OAKLAND, CA");
    airPortCodes.push_back("MAF	ODESSA/MIDLAND");
    airPortCodes.push_back("OKC	OKLAHOMA CITY, OK");
    airPortCodes.push_back("OMA	OMAHA, NE");
    airPortCodes.push_back("ONT	ONTARIO, CA");
    airPortCodes.push_back("SNA	ORANGE COUNTY, CA");
    airPortCodes.push_back("MCO	ORLANDO, FL");
    airPortCodes.push_back("OWB	OWENSBORO, KY");
    airPortCodes.push_back("OXR	OXNARD, CA");
    airPortCodes.push_back("PSP	PALM SPRINGS, CA");
    airPortCodes.push_back("PNS	PENSACOLA, FL");
    airPortCodes.push_back("PIA	PEORIA, IL");
    airPortCodes.push_back("PHL	PHILADELPHIA, PA");
    airPortCodes.push_back("PHX	PHOENIX, AZ");
    airPortCodes.push_back("PIT	PITTSBURGH, PA");
    airPortCodes.push_back("PWM	PORTLAND, ME");
    airPortCodes.push_back("PDX	PORTLAND, OR");
    airPortCodes.push_back("PVD	PROVIDENCE, RI");
    airPortCodes.push_back("RDU	RALEIGH-DURHAM, NC");
    airPortCodes.push_back("RNO	RENO, NV");
    airPortCodes.push_back("RIC	RICHMOND, VA");
    airPortCodes.push_back("RST	ROCHESTER, MN");
    airPortCodes.push_back("ROC	ROCHESTER, NY");
    airPortCodes.push_back("RFD	ROCKFORD, IL");
    airPortCodes.push_back("SMF	SACRAMENTO, CA");
    airPortCodes.push_back("SLC	SALT LAKE CITY, UT");
    airPortCodes.push_back("SJT	SAN ANGELO, TX");
    airPortCodes.push_back("SAT	SAN ANTONIO, TX");
    airPortCodes.push_back("SAN	SAN DIEGO, CA");
    airPortCodes.push_back("SFO	SAN FRANCISCO, CA");
    airPortCodes.push_back("SJC	SAN JOSE, CA");
    airPortCodes.push_back("SJU	SAN JUAN, PR");
    airPortCodes.push_back("SBP	SAN LUIS OBISPO, CA");
    airPortCodes.push_back("SBA	SANTA BARBARA, CA");
    airPortCodes.push_back("SMX	SANTA MARIA, CA");
    airPortCodes.push_back("SRQ	SARASOTA, FL");
    airPortCodes.push_back("SEA	SEATTLE, WA");
    airPortCodes.push_back("SHV	SHREVEPORT, LA");
    airPortCodes.push_back("FSD	SIOUX FALLS, SD");
    airPortCodes.push_back("SBN	SOUTH BEND, IN");
    airPortCodes.push_back("GEG	SPOKANE, WA");
    airPortCodes.push_back("SPI	SPRINGFIELD, IL");
    airPortCodes.push_back("SGF	SPRINGFIELD, MO");
    airPortCodes.push_back("STL	ST. LOUIS, MO");
    airPortCodes.push_back("SYR	SYRACUSE, NY");
    airPortCodes.push_back("TPA	TAMPA, FL");
    airPortCodes.push_back("TXK	TEXARKANA, AR");
    airPortCodes.push_back("TOL	TOLEDO, OH");
    airPortCodes.push_back("TVC	TRAVERSE CITY, MI");
    airPortCodes.push_back("TUS	TUCSON, AZ");
    airPortCodes.push_back("TUL	TULSA, OK");
    airPortCodes.push_back("TCL	TUSCALOOSA, AL");
    airPortCodes.push_back("TYR	TYLER, TX");
    airPortCodes.push_back("EGE	VAIL, CO");
    airPortCodes.push_back("ACT	WACO, TX");
    airPortCodes.push_back("IAD	WASHINGTON DC/DULLES");
    airPortCodes.push_back("DCA	WASHINGTON DC/NATIONAL");
    airPortCodes.push_back("CWA	WAUSAU/STEVENS POINT, WI");
    airPortCodes.push_back("PBI	WEST PALM BEACH, FL");
    airPortCodes.push_back("HPN	WESTCHESTER COUNTY, NY");
    airPortCodes.push_back("ICT	WICHITA, KS");
    airPortCodes.push_back("SPS	WICHITA FALLS, TX");
    airPortCodes.push_back("AVP	WILKES BARRE, PA");
    airPortCodes.push_back("ORH	WORCESTER, M");

    // this is a wierd clean up!, skip the first one
    for(int i = 1 ; i < airPortCodes.size(); i++)
         airPortCodes[i] = airPortCodes[i].replace(3,1,QString("    "));
}

QStringList AirportList::getAirportList(){

    return airPortCodes;

}

int AirportList::getIndexValue(QString value){

    QString pattern = "^"+value.trimmed()+".*$";
    return airPortCodes.indexOf(QRegExp(pattern,Qt::CaseInsensitive));
}

DelayReasons::DelayReasons(){
    reasons.push_back("");
    reasons.push_back("Cx");
    reasons.push_back("Mx");
}

QStringList DelayReasons::getDelayReasonList(){

    return reasons;
}

int DelayReasons::getIndexValue(QString value){
    QString pattern = "^"+value.trimmed()+".*$";
    return reasons.indexOf(QRegExp(pattern,Qt::CaseInsensitive));
}

Penalty::Penalty(){

    penaltyReasons.push_back("");
    penaltyReasons.push_back("Delay");
    penaltyReasons.push_back("Int");
    penaltyReasons.push_back("M");
    penaltyReasons.push_back("P");
}



QStringList Penalty::getPenaltyList(){
    return penaltyReasons;
}

int Penalty::getIndexValue(QString value){
    QString pattern = "^"+value.trimmed()+".*$";
    return penaltyReasons.indexOf(QRegExp(pattern,Qt::CaseInsensitive));
}



/*
 * This is a Flight container constructor, See the class for more details
 *
 */
FlightContainer::FlightContainer(){

    isFirstLeg = false;
    isPartialDelayedFlight = false;
    routeIndex  = -1;
    subRouteIndex = -1;
    pointedSubRouteIndex = -1;
    hasFollowingContinuousLeg = false;
    isContinuousCancelledLeg = false;
}




AllRoutes* XLSFileIO::readXLSFile(QString fileName){
    AllRoutes *readRoutes = NULL ;
    return readRoutes;
}

bool XLSFileIO::writeXLSFile(QString fileName, AllRoutes &allRoutes){
    return true;
}





// THE DOM MODEL, WORKS FOR SMALLER XML FILES, USE SAX IF YOU NEED TO PARSE LARGER FILES
AllRoutes* XMLFileIO::readXMLFile(QString fileName){

    // This is the generic message variable
    QString msg;

    // Object to be populated with the XML objects
    AllRoutes *readRoutes = NULL;

    // The document
    QDomDocument document;

    //Load the file
    QFile file(fileName);
    // check to see if the file could be opened correctly in the read mode and also the document tree could be formed
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text) || !document.setContent(&file)){
        msg = "Status: Error! Msg: Unable to open or read XML file. Module Name:  XMLFileIO::readXMLFile \n" ;
        qDebug() << msg;
        //guiDisplayMessage("Error",msg,this);
        return readRoutes;       // This will return null
    }
    // close file after everything has gone fine.
    file.close();

    readRoutes = new AllRoutes();
    Route route;
    SubRoute subRoute;
    Leg leg;
    std::vector<Leg> legs;
    std::vector<Route> routes;
    std::vector<SubRoute> subRoutes;
    //get the root elememt, The template has ROUTES here
    QDomNodeList routeList = document.firstChildElement().elementsByTagName("Route");

    for(int i = 0 ; i < routeList.count(); i++){
        QDomNode routeNode = routeList.at(i);
        // Getting the Parent Route
        if(routeNode.isElement() && routeNode.toElement().nodeName() == "Route"){
            QDomElement routeElement = routeNode.toElement();
            //get the route name <Route name="ABC">
            route.setRouteName(routeElement.attributeNode("name").value());
            // get the Tail from Tails(xml) from Route, which is subroute in code
            QDomNodeList tailList = routeElement.elementsByTagName("Tails").at(0).toElement().elementsByTagName("Tail");
            subRoutes.clear();
            for(int j = 0 ; j < tailList.count(); j++){
                // extract each Tail
                QDomNode tailNode = tailList.at(j);
                if(tailNode.isElement() && tailNode.toElement().nodeName() == "Tail"){
                    // convert Tail Node to Tail Element
                    QDomElement tailElement = tailNode.toElement();
                    subRoute.setTailNo(tailElement.attributeNode("tailNo").value());
                    //get all the legs in legList

                    legs.clear();
                    QDomNodeList legList = tailElement.elementsByTagName("Leg");
                    for(int k = 0 ; k < legList.count(); k++){
                        QDomNode legNode = legList.at(k);
                        if(legNode.isElement() && legNode.toElement().nodeName() == "Leg"){
                            QDomElement legElement = legNode.toElement();
                            // get the values of a leg here, i.e. Source, Destination and Meta Info
                            // Every leg has a mandatory Source, Destination
                            // Collect Source here
                            leg.setDeptAP(legElement.childNodes().at(0).toElement().attributeNode("airportName").value()) ;
                            leg.setDeptTime(legElement.childNodes().at(0).toElement().attributeNode("departureTime").value());
                            // Collect Destination here
                            leg.setArrivalAP(legElement.childNodes().at(1).toElement().attributeNode("airportName").value()) ;
                            leg.setArrivalTime(legElement.childNodes().at(1).toElement().attributeNode("arrivalTime").value());
                            // Collect Meta Info here
                            std::vector<QString> factor;
                            std::vector<QString> cxTail;
                            std::vector<QString> penalty;
                            std::vector<QString> cost;
                            QDomNodeList legMetaList = legElement.elementsByTagName("Meta");
                            for(int l = 0 ; l < legMetaList.count(); l++){
                                QDomNode metaNode = legMetaList.at(l);
                                if(metaNode.isElement() && metaNode.toElement().nodeName() == "Meta"){
                                    QDomElement metaElement = metaNode.toElement();
                                    // factor is id in attribute of leg in template XML
                                    factor.push_back(metaElement.attributeNode("id").value());
                                    cxTail.push_back(metaElement.attributeNode("isCx").value());
                                    penalty.push_back(metaElement.attributeNode("penaltyId").value());
                                    cost.push_back(metaElement.attributeNode("penaltyCost").value());
                                }
                            }
                            leg.setFactor(factor);
                            leg.setCxTail(cxTail);
                            leg.setPenalty(penalty);
                            leg.setCost(cost);
                            legs.push_back(leg);
                        }
                    }
                }
                subRoute.setLegs(legs);
                subRoutes.push_back(subRoute);
            }

        }
        route.setSubRoutes(subRoutes);
        routes.push_back(route);
    }

    readRoutes->setRoutes(routes);
    return readRoutes;
}

// THE DOM MODEL
bool XMLFileIO::writeXMLFile(AllRoutes &allRoutes){


    Route tempRoute;
    SubRoute tempSubRoute;
    Leg tempLeg;


    //create a document
    QDomDocument document;

    //create a root node
    QDomElement routes = document.createElement("Routes");

    //append the root node to the document
    document.appendChild(routes);

    for(unsigned int i = 0 ; i < allRoutes.getRoutes().size(); i++){
        tempRoute = allRoutes.getRoutes().at(i);

        QDomElement route = document.createElement("Route");
        route.setAttribute("name",tempRoute.getRouteName());
        routes.appendChild(route);

        QDomElement tails = document.createElement("Tails");
        route.appendChild(tails);

        for(unsigned  int j = 0 ; j < tempRoute.getSubRoutes().size(); j++){

            tempSubRoute = tempRoute.getSubRoutes().at(j);

            QDomElement tail = document.createElement("Tail");
            tail.setAttribute("tailNo",tempSubRoute.getTailNo());
            tails.appendChild(tail);

            for(unsigned int k = 0 ; k < tempSubRoute.getLegs().size(); k++){

                tempLeg = tempSubRoute.getLegs().at(k);

                QDomElement leg = document.createElement("Leg");
                tail.appendChild(leg);

                QDomElement source = document.createElement("Source");
                source.setAttribute("airportName",tempLeg.getDeptAP());
                source.setAttribute("departureTime",tempLeg.getDeptTime());

                QDomElement destination = document.createElement("Destination");
                destination.setAttribute("arrivalTime",tempLeg.getArrivalTime());
                destination.setAttribute("airportName",tempLeg.getArrivalAP());


                leg.appendChild(source);
                leg.appendChild(destination);

                for(int l = 0 ; l < tempLeg.getFactor().size() ; l++){

                    QDomElement meta = document.createElement("Meta");
                    meta.setAttribute("id",tempLeg.getFactor().at(l));
                    meta.setAttribute("isCx",tempLeg.getCxTail().at(l));
                    meta.setAttribute("penaltyId",tempLeg.getPenalty().at(l));
                    meta.setAttribute("penaltyCost",tempLeg.getCost().at(l));
                    leg.appendChild(meta);

                }
            }

        }
    }

    QString saveFileName = QFileDialog::getSaveFileName(NULL,"save as xml","","xml file(*.xml)");
    QFile fileObj(saveFileName);
    while(!fileObj.open(QIODevice::WriteOnly | QIODevice::Text)){
        QErrorMessage message;
        message.showMessage("Unable to open xml File, Please check the path or filename");
        message.exec();
        saveFileName = QFileDialog::getSaveFileName(NULL,"save as xml","","xml file(*.xml)");
        fileObj.setFileName(saveFileName);
    }

    QTextStream stream(&fileObj);
    stream << document.toString();
    fileObj.close();


    return true;
}

Leg::Leg(){
    isFirstLeg = false;
}

QString Leg::getDeptTime() const
{
    return deptTime;
}

void Leg::setDeptTime(const QString &value)
{
    deptTime = value;
}

QString Leg::getArrivalAP() const
{
    return arrivalAP;
}

void Leg::setArrivalAP(const QString &value)
{
    arrivalAP = value;
}

QString Leg::getArrivalTime() const
{
    return arrivalTime;
}

void Leg::setArrivalTime(const QString &value)
{
    arrivalTime = value;
}


std::vector<QString> Leg::getFactor() const
{
    return factor;
}

void Leg::setFactor(const std::vector<QString> &value)
{
    factor = value;
}

std::vector<QString> Leg::getCxTail() const
{
    return cxTail;
}

void Leg::setCxTail(const std::vector<QString> &value)
{
    cxTail = value;
}

std::vector<QString> Leg::getPenalty() const
{
    return penalty;
}

void Leg::setPenalty(const std::vector<QString> &value)
{
    penalty = value;
}

std::vector<QString> Leg::getCost() const
{
    return cost;
}

void Leg::setCost(const std::vector<QString> &value)
{
    cost = value;
}

QString Leg::getDeptAP() const
{
    return deptAP;
}

void Leg::setDeptAP(const QString &value)
{
    deptAP = value;
}


QString SubRoute::getTailNo() const
{
    return tailNo;
}

void SubRoute::setTailNo(const QString &value)
{
    tailNo = value;
}
std::vector<Leg> SubRoute::getLegs() const
{
    return legs;
}

void SubRoute::setLegs(const std::vector<Leg> &value)
{
    legs = value;
}


QString Route::getRouteName() const
{
    return routeName;
}

void Route::setRouteName(const QString &value)
{
    routeName = value;
}
std::vector<SubRoute> Route::getSubRoutes() const
{
    return subRoutes;
}

void Route::setSubRoutes(const std::vector<SubRoute> &value)
{
    subRoutes = value;
}

std::vector<Route> AllRoutes::getRoutes() const
{
    return routes;
}

void AllRoutes::setRoutes(const std::vector<Route> &value)
{
    routes = value;
}


AirportNameGLCoordinate::AirportNameGLCoordinate()
{
    isContinuousLeg = false;
}
