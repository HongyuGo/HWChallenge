#include "Robot.h"
void Robot::SetPalstance(double _Palstance){
    if(_Palstance > PI || _Palstance < -PI){
        cerr << "The Palstance entered is out of range" << endl;
        exit(1);
    }
    Palstance = _Palstance;
}
void Robot::SetLinearSpeed(vector<double>& _LinearSpeed){
    LinearSpeed = _LinearSpeed;
    double _RealSpeed = sqrt(LinearSpeed[0] * LinearSpeed[0] + LinearSpeed[1] * LinearSpeed[1]);
    RealSpeed = _RealSpeed > MaxFoward ? MaxFoward : _RealSpeed;
    RealSpeed = _RealSpeed < MinBack ? MinBack : _RealSpeed;
}
void Robot::CalculatePhysicalParams(){
    Area = Radius * Radius * PI;
    Quality = Area * Density;
    Acceleration = Traction / Quality;
}
void Robot::ShowRobot() const {
    cerr << "WorkBenchIDClose : " << WorkBenchID << " TypeArticleCarry : " << TypeArticleCarry << " TimeValueCoeffcient : " << TimeValueCoeffcient 
         << " CollisionValueCoeffcient : "  << CollisionValueCoeffcient << endl;
    cerr << "Palstance : " << Palstance << " xSpeed : " << LinearSpeed[0] << " ySpeed : " << LinearSpeed[1] << " RealSpeed : " << RealSpeed 
         << " Towards : " << Towards << " xAxis : " << setw(5) << Axis[0] << " yAxis : " << setw(5) << Axis[1] << " Radius : " << Radius << endl;
}

void RobotMemRelease(const vector<Robot*> _RobotVec){
    int i;
    for(i = 0; i < RobotNum; i++){
        delete _RobotVec[i];
    }
}