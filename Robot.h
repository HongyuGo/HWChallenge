#pragma once
#include "Params.h"

class Robot
{
public:
    static constexpr double NormalRadius = 0.45;
    static constexpr double HoldingRadius = 0.53;
    static constexpr double Density = 20.0;
    static constexpr double MaxFoward = 6.0;
    static constexpr double MinBack = -2.0;
    static constexpr double MaxRotaing = PI;
    static constexpr double MaxTraction = 250.0;
    static constexpr double MaxTorque = 50.0;
    int WorkBenchID = -1;
    int RobotID = -1;
    int TypeArticleCarry = 0; // 0 : no product, [1~7] : product id
    double TimeValueCoeffcient = 0.0;
    double CollisionValueCoeffcient = 0.0;
    double Palstance = 0.0;
    vector<double> LinearSpeed{0.0, 0.0};
    double RealSpeed = 0.0;
    double Towards = 0.0;
    vector<double> Axis{0.0, 0.0};
    double Radius = NormalRadius;
    double Area = Radius * Radius * PI;
    double Quality = Area * Density;
    double Traction = 0.0;
    double Torque = 0.0;
    double Acceleration = 0.0;
    int WantToCloseWBID = 0;
    double RobotWorkBenchDis = 0.0;
    double AngleDifference = 0.0;
    int WantTOCloseWBKind = 0;
    int ProductFlag = 0;  // 0 : no product, [1~7] : product id
    int HaveTarget = -1;  // 没有要去的工作台，0~k是要去的工作台ID
    int UrgentWork = -1;  //-1 : no urgent work, [0~K] : urgent workbench id
    int BuySellState = 0; // 0：buy, 1:sell
public:
    Robot() {}
    int &GetWorkBenchID() { return WorkBenchID; }
    int &GetTypeArticleCarry() { return TypeArticleCarry; }
    double &GetTimeValueCoeffcient() { return TimeValueCoeffcient; }
    double &GetCollisionValueCoeffcient() { return CollisionValueCoeffcient; }
    double &GetPalstance() { return Palstance; }
    vector<double> &GetLinearSpeed() { return LinearSpeed; }
    double &GetRealSpeed() { return RealSpeed; }
    double &GetTowards() { return Towards; }
    vector<double> &GetAxis() { return Axis; }
    double &GetRadius() { return Radius; }
    void SetPalstance(double _Palstance);
    void SetLinearSpeed(vector<double> &_LinearSpeed);
    int &GetWantToCloseWBID() { return WantToCloseWBID; }
    double &GetRobotWorkBenchDis() { return RobotWorkBenchDis; }
    double &GetAngleDifference() { return AngleDifference; }
    void CalculatePhysicalParams();
    void ShowRobot() const;
    ~Robot() {}
};

void RobotMemRelease(const vector<Robot *> _RobotVec);