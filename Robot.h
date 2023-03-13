#pragma once
#include "Params.h"
class Robot{
    private:
        static constexpr double NormalRadius = 0.45;
        static constexpr double HoldingRadius = 0.53;
        static constexpr double Density = 20.0;
        static constexpr double MaxFoward = 6.0;
        static constexpr double MinBack = -2.0;
        static constexpr double MaxRotaing = PI;
        int WorkBenchID = -1;
        int TypeArticleCarry = 0;
        double TimeValueCoeffcient = 0.0;
        double CollisionValueCoeffcient = 0.0;
        double Palstance = 0.0;
        vector<double> LinearSpeed{0.0, 0.0};
        double RealSpeed = 0.0;
        double Towards = 0.0;
        vector<double> Axis{0.0, 0.0};
        double Radius = NormalRadius;
    public:
        Robot(){}
        int& GetWorkBenchID(){return WorkBenchID;}
        int& GetTypeArticleCarry(){return TypeArticleCarry;}
        double& GetTimeValueCoeffcient(){return TimeValueCoeffcient;}
        double& GetCollisionValueCoeffcient(){return CollisionValueCoeffcient;}
        double& GetPalstance(){return Palstance;}
        vector<double>& GetLinearSpeed(){return LinearSpeed;}
        double& GetRealSpeed(){return RealSpeed;}
        double& GetTowards(){return Towards;}
        vector<double>& GetAxis(){return Axis;}
        double& GetRadius(){return Radius;}
        void SetPalstance(double _Palstance);
        void SetLinearSpeed(vector<double>& _LinearSpeed);
        void ShowRobot()const;
        ~Robot(){}
};

void RobotMemRelease(const vector<Robot*> _RobotVec);