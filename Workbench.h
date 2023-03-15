#pragma once
#include "Params.h"
#include "Robot.h"
class Workbench{
    public:
        enum{WORKTIME1 = 50, WORKTIME2 = 50, WORKTIME3 = 50, 
             WORKTIME4 = 500, WORKTIME5 = 500, WORKTIME6 = 500,
             WORKTIME7 = 1000, WORKTIME8 = 1, WORKTIME9 = 1};
        enum{BUYINGPRICE1 = 3000, BUYINGPRICE2 = 4400, BUYINGPRICE3 = 5800,
             BUYINGPRICE4 = 15400,BUYINGPRICE5 = 17200,BUYINGPRICE6 = 19200,
             BUYINGPRICE7 = 76000,BUYINGPRICE8 = -1,   BUYINGPRICE9 = -1};
        enum{SELLINGPRICE1 = 6000, SELLINGPRICE2 = 7600, SELLINGPRICE3 = 9200,
             SELLINGPRICE4 = 22500,SELLINGPRICE5 = 25000,SELLINGPRICE6 = 27500,
             SELLINGPRICE7 = 105000,SELLINGPRICE8 = -1,  SELLINGPRICE9 = -1};
        
        int WorkBenchKind;//the kind of workbench
        int Role;//0:producer, 1:synthesis, 2:saler
        vector<int> Material;
        int MaterialStatus;
        int RestWorkTime;//the remaining production time
        int ProductKind;//the kind of resulting product, 0:do not produce products
        vector<double> Axis;
        int WorkBenchID;
        int BuyingPrice;
        int SellingPrice;
        int ProductStatus= 0;//1:have, 0:not have
        int RobotScheduled = -1;
    public:
        Workbench(char _WorkBenchKind, const vector<double>& _axis, int _WorkBenchID);
        int& GetWorkBenchKind(){return WorkBenchKind;}
        int& GetWorkBenchID(){return WorkBenchID;}
        int& GetRole(){return Role;}
        vector<int>& GetMaterial(){return Material;}
        int& GetMaterialStatus(){return MaterialStatus;}
        int& GetRestWorkTime(){return RestWorkTime;}
        int& GetProductKind(){return ProductKind;}
        vector<double>& GetAxis(){return Axis;}
        int& GetProductStatus(){return ProductStatus;}
        bool HaveLock(int CarryType, vector<Robot*>& _Robot);
        void ShowWorkBench()const;
        ~Workbench(){};

};

void WorkBenchAndRobotRead(const string& _InitString, vector<Workbench*>& _WorkBenchVec, vector<Robot*>& _RobotVec, int yaxis);
void WorkBenchAllocate(const vector<Workbench*>& _WorkBenchVec, vector<vector<Workbench*>> &_WorkBenchSelf);
void WorkBenchMemRelease(const vector<Workbench*>& _WorkBenchVec);