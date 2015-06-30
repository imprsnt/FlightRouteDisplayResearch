#ifndef CUSTOMCLASSES_H
#define CUSTOMCLASSES_H

#include <QStringList>
#include <QString>
#include <vector>
#include <QMessageBox>
#include <qgl.h>
#include <QTime>

/*
void guiDisplayMessage(QString title, QString msg, QWidget *parent){
    QMessageBox::critical(parent,title,msg,QMessageBox::Ok);
}
*/
//////////////////////// CONTAINER CLASSES BEGIN ////////////////////////////////////////////////////////////
// These classes are the container classes for reading and writing XLS sheet
class Leg{

private:
    QString deptAP;
    QString deptTime;
    QString arrivalAP;
    QString arrivalTime;
    std::vector<QString> factor;
    std::vector<QString> cxTail;
    std::vector<QString> penalty;
    std::vector<QString> cost;

public:

    bool isFirstLeg;        // This is for displaying visual feedback text on both sides of the leg

    Leg();

    QString getDeptAP() const;
    void setDeptAP(const QString &value);

    QString getDeptTime() const;
    void setDeptTime(const QString &value);

    QString getArrivalAP() const;
    void setArrivalAP(const QString &value);

    QString getArrivalTime() const;
    void setArrivalTime(const QString &value);


    std::vector<QString> getFactor() const;
    void setFactor(const std::vector<QString> &value);

    std::vector<QString> getCxTail() const;
    void setCxTail(const std::vector<QString> &value);

    std::vector<QString> getPenalty() const;
    void setPenalty(const std::vector<QString> &value);

    std::vector<QString> getCost() const;
    void setCost(const std::vector<QString> &value);
};

class SubRoute{

private:
    QString tailNo;
    std::vector<Leg> legs;

public:

    std::vector<Leg> getLegs() const;
    void setLegs(const std::vector<Leg> &value);
    QString getTailNo() const;
    void setTailNo(const QString &value);
};

class Route{

private:
    QString routeName;
    std::vector<SubRoute> subRoutes;

public:

    std::vector<SubRoute> getSubRoutes() const;
    void setSubRoutes(const std::vector<SubRoute> &value);
    QString getRouteName() const;
    void setRouteName(const QString &value);
};

class AllRoutes{

private:

    std::vector<Route> routes;

public:

    std::vector<Route> getRoutes() const;
    void setRoutes(const std::vector<Route> &value);
};


//////////////////////// CONTAINER CLASSES END ////////////////////////////////////////////////////////////


///////////////////////  MAKE CLASSES FOR DISPLAY PURPOSE IN OPENGL BEGIN //////////////////////////////////

class DisplayGLSubRoute{
public:
    GLfloat yLevelTop;
    GLfloat yLevelBottom;
    GLfloat unitHeightValue;  // this parameter needs to be multiplied by heightmultiplier to get the actual value,
                              // so this value gives us a count of how many rows are contained in a subroute
};

class DisplayGLRoute{
public:
    GLfloat yLevelTop;
    GLfloat yLevelBottom;
    GLfloat totalUnitHeightOfSubRoutes;
    std::vector<DisplayGLSubRoute> displayGLSubRoute;

};

///////////////////////  MAKE CLASSES FOR DISPLAY PURPOSE IN OPENGL END //////////////////////////////////





///////////////////////// MAKE UTILITY DATABASE CLASSES BEGIN /////////////////////////////////////////////
class AirportList{

private:
    QStringList airPortCodes;

public:
    QStringList getAirportList();
    int getIndexValue(QString value);
    AirportList();
};

class DelayReasons{
private:
    QStringList reasons;
public:
    DelayReasons();
    QStringList getDelayReasonList();
    int getIndexValue(QString value);
};

class Penalty{
private:
    QStringList penaltyReasons;
public:
    Penalty();
    QStringList getPenaltyList();
    int getIndexValue(QString value);
};


// Text and Visual Flight Info Container Classes

class FlightContainer{
public:
    QString startTime;
    QString endTime;
    QString source;
    QString destination;
    bool    isFirstLeg;             // important for setting up text on both side, coz only the first leg will have text displayed on both sides
    bool    isPartialDelayedFlight; // for multi coloring purpose
    QTime partStartingTime;         // starting ------ mid ------------end
    QTime partSMidTime;             // |                |                |
    QTime partSEndTime;             // | normal color   | delay color    |
                                    // -----------------------------------
    int routeIndex;                  // This is for drawing Cx Lines
    int subRouteIndex;               // This is for drawing Cx Lines
    int pointedSubRouteIndex;        // This is for drawing Cx Lines

    std::vector<QString> isCxOrMx;
    std::vector<QString> penaltyCode;
    std::vector<QString> penaltyCost;

    bool hasFollowingContinuousLeg;  // This is used to place the AP names in between continuous legs
    bool isContinuousCancelledLeg; // This to display AP names on either side of the leg for discrete legs and only on the right for continuous leg

    FlightContainer();
};

class AirportNameGLCoordinate{
public:

    QString apName;
    GLfloat xLevel;     // x value of the text
    GLfloat yLevel;     // y coordinate of the text
    int     legIndex;
    int     subRouteIndex;
    bool    isContinuousLeg;    // used for cancelled flight Airport name display...

    AirportNameGLCoordinate();
};


///////////////////////// MAKE UTILITY DATABASE CLASSES END /////////////////////////////////////////////



///////////////////////// FILE IO CLASSES START /////////////////////////////////////////////////////////

// class to read and write XLS file
class XLSFileIO{
public:
    AllRoutes* readXLSFile(QString fileName);
    bool writeXLSFile(QString fileName, AllRoutes &allRoutes);
};

// class to read and write XLS file
class XMLFileIO{
public:
    AllRoutes* readXMLFile(QString fileName);
    bool writeXMLFile(AllRoutes &allRoutes);
};

///////////////////////// FILE IO CLASSES END /////////////////////////////////////////////////////////


#endif // CUSTOMCLASSES_H
