#include "Robot.h"
#include "Workbench.h"
#include <cstring>
#include "Params.h"
#define MAPLEN (100)
string map[MAPLEN];
vector<int> WorkBenchNum(10,0);
vector<Workbench*>WorkBenchVec;
vector<vector<Workbench*>> WorkBenchSelf(10);
vector<Robot*>RobotVec;
queue<int> Qfour;
queue<int> Qfive;
queue<int> Qsix;
int Money;

// char map[MAPLEN][MAPLEN];

bool MapInit() {
    char line[1024];
    int i = 0;
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == 'O' && line[1] == 'K') {
            cerr << "WorkBenchVec.size = " << WorkBenchVec.size() << endl;
            WorkBenchAllocate(WorkBenchVec, WorkBenchSelf);
            for(i = 1 ; i < 10; i++){
                cerr << "Workbench" << i << "'s number is " << WorkBenchSelf[i].size() << endl;
            }
            cerr << "------------------------------------------------------------------" << endl;
            cerr << "Robot" << endl;
            for(i = 0; i < RobotNum; i++){
                RobotVec[i]->ShowRobot();
            }
            return true;
        }
        map[i] = line;
        // cerr << map[i] << endl;
        // exit(1);
        WorkBenchAndRobotRead(map[i], WorkBenchVec, RobotVec, i); 
        i++;//iterative updates
    }
    cerr << "An error occurred with the readUntilOK function." << endl;
    exit(1);
    return false;
}
bool readUntilOKtet(){
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        // cerr << line << endl;
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
    }
    return false;
}

bool ReadInfo(int _WorkBenchNum, vector<Workbench*> _WorkBenchVec, vector<Robot*> _RobotVec){
    char line[10];
    int i = 0;
    vector<double> _axis(2,0.0);
    vector<double> _speed(2,0.0);
    #if (SHOW == 1)
        cerr << "----------------------------------------------" << endl;
        cerr << "WorkBench" << endl;
    #endif
    for(i = 0; i < _WorkBenchNum; i++){
        cin >> _WorkBenchVec[i]->GetWorkBenchKind();
        cin >> _axis[0] >> _axis[1];
        _WorkBenchVec[i] -> GetAxis() = _axis;
        cin >> _WorkBenchVec[i] -> GetRestWorkTime() >> _WorkBenchVec[i] -> GetMaterialStatus() >> _WorkBenchVec[i] -> GetProductStatus();
        // cerr << "Material Status" << _WorkBenchVec[i]->MaterialStatus << endl;
        #if (SHOW == 1)
            _WorkBenchVec[i] -> ShowWorkBench();
        #endif
    }
    #if (SHOW == 1)
        cerr << "----------------------------------------------" << endl;
        cerr << "Robot" << endl;
    #endif
    for(i = 0; i < 4; i++){
        cin >> _RobotVec[i] -> GetWorkBenchID() >> _RobotVec[i] -> GetTypeArticleCarry() >> _RobotVec[i] -> GetTimeValueCoeffcient()
            >> _RobotVec[i] -> GetCollisionValueCoeffcient() >> _RobotVec[i] -> GetPalstance();
        cin >> _speed[0] >> _speed[1];
        _RobotVec[i] -> SetLinearSpeed(_speed);
        cin >> _RobotVec[i] -> GetTowards();
        cin >> _axis[0] >> _axis[1];
        _RobotVec[i] -> GetAxis() = _axis;
        #if (SHOW == 1)
            _RobotVec[i] -> ShowRobot();
        #endif
    }
    cin.get();
    if(fgets(line, sizeof line, stdin)) {
        cerr << line << endl;
        if (line[0] == 'O' && line[1] == 'K') {
            return true;
        }
        else{
            cerr << "Get infomation error." << endl;
            exit(1);
        }
    }
    return false;
}

double operator-(const vector<double> _left, const vector<double> _right){
    return sqrt((_left[0] - _right[0]) * (_left[0] - _right[0]) + 
                (_left[1] - _right[1]) * (_left[1] - _right[1]));
}
ostream& operator<<(ostream& os, const vector<double> _axis){
    os << "_axisX : " << _axis[0] << "_axisY : " << _axis[1];
    return os;
}

//find the robot id nearest to the workbench
int FindMinDistance(vector<Robot*> _Robot, Workbench* _WorkBench){
    int i;
    int WB2RMinID = _Robot.size() - 1;
    vector<double> _axis = _WorkBench->GetAxis();
    double MinDistance = 1e10;
    for(i = 0; i < _Robot.size(); i++){
        double DistanceTmp = _axis - _Robot[i]->GetAxis();
        if(DistanceTmp < MinDistance){
            MinDistance = DistanceTmp;
            WB2RMinID= i;
        }
    }
    return WB2RMinID;
}

int FindMinDistanceTargetWB(Robot* _Robot, vector<Workbench*> _WorkBenchVec){
    int i;
    int MinID = _WorkBenchVec.size() - 1;
    vector<double> _axis = _Robot->GetAxis();
    double MinDistance = 1e10;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
        // cerr << _WorkBenchVec[i] -> RobotScheduled << endl;
        // exit(1);
        if(DistanceTmp < MinDistance && _WorkBenchVec[i] -> RobotScheduled == 0){
            // cerr << "Min test" << endl;
            MinDistance = DistanceTmp;
            MinID= i;
        }
    }
    // cerr << "MinID" << MinID << endl;
    // exit(1);
    if(_WorkBenchVec[MinID] -> RobotScheduled == 0)
        _WorkBenchVec[MinID] -> RobotScheduled = 1;
    // cerr << "MinID " << MinID << endl;
    // cerr << "TargetWB " << _WorkBenchVec[MinID]->WorkBenchID << endl;
    return MinID;
}

vector<int> FindVectorWBMinID(Robot* _Robot,vector<Workbench*>& _WorkBenchVec){
    int i;
    int VectorWBMinID = _WorkBenchVec.size() - 1;
    vector<double> _axis = _Robot->GetAxis();
    double MinDistance = 1e10;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
        if(DistanceTmp < MinDistance /*&& _WorkBenchVec[i] -> RobotScheduled == 0*/){
            MinDistance = DistanceTmp;
            VectorWBMinID= i;
        }
    }
    if(_WorkBenchVec[VectorWBMinID] -> RobotScheduled == 0)
        _WorkBenchVec[VectorWBMinID] -> RobotScheduled = 1;
    vector<int> ID(2,0);
    ID[0] = VectorWBMinID;
    ID[1] = _WorkBenchVec[VectorWBMinID] -> WorkBenchID;
    return ID;
}
vector<int> FindVectorWBMinIDSell(Robot* _Robot,vector<Workbench*>& _WorkBenchVec, int _Origin){
    int i;
    int VectorWBMinID =  _WorkBenchVec.size() - 1;
    vector<double> _axis = _Robot->GetAxis();
    double MinDistance = 1e10;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        int flag = 0;
        double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
        int Material = _WorkBenchVec[i]->MaterialStatus;
        cerr << "Material: "<< Material << endl; 
        if(Material == 0){
            flag = 0;
        }else{
            if(Material & (1 << _Origin)){
                flag = 1;
            }else{
                flag = 0;
            }
        }
        cerr << "flag : " << flag << endl;
        if(DistanceTmp < MinDistance && flag == 0 && _WorkBenchVec[i] -> RobotScheduled == 0){
            MinDistance = DistanceTmp;
            VectorWBMinID = i;
        }
    }
    // if(_WorkBenchVec[VectorWBMinID] -> RobotScheduled == 0)
    //     _WorkBenchVec[VectorWBMinID] -> RobotScheduled = 1;
    if(WorkBenchVec[_WorkBenchVec[VectorWBMinID] -> WorkBenchID] -> RobotScheduled == 0)
        WorkBenchVec[_WorkBenchVec[VectorWBMinID] -> WorkBenchID] -> RobotScheduled  =  1;
    vector<int> ID(2,0);
    if(MinDistance == 1e10)
        ID[0] = -1;
    else
        ID[0] = VectorWBMinID;
    ID[1] = _WorkBenchVec[VectorWBMinID] -> WorkBenchID;
    return ID;
}
double CalculateDistance(Robot* _Robot, vector<Workbench*> _WorkBenchVec){
    int i;
    int MinID;
    vector<double> _axis = _Robot->GetAxis();
    if(_Robot->HaveTarget == -1){
        MinID = FindMinDistanceTargetWB(_Robot, _WorkBenchVec);
        _Robot->HaveTarget = MinID;
    }
    else    
        MinID = _Robot->HaveTarget;
    // cerr << "MinID" << MinID << endl;
    // cerr << "teststetstes" << endl;
        // _Robot->HaveTargetWBFlag = 1;
    // }
    int flag = 0;
    double XDistance =  _WorkBenchVec[MinID]->GetAxis()[0] - _axis[0];
    double YDistance =  _WorkBenchVec[MinID]->GetAxis()[1] - _axis[1];
    int quadrant = 0;
    if(YDistance > 0.0 && XDistance > 0.0)
        quadrant = 1;
    else if(YDistance < 0.0 && XDistance > 0.0)
        quadrant = 4;
    else if(YDistance > 0.0 && XDistance < 0.0)
        quadrant = 2;
    else
        quadrant = 3;

    double Angle = atan2(YDistance, XDistance);
    // cerr << "Angle" << Angle << endl;
    double AngleDifference = abs(Angle - _Robot->GetTowards());
    if(abs(AngleDifference) < 0.5)
        flag = Angle - _Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
    else    
        flag = 1;
    double RealDis =  sqrt(XDistance * XDistance + YDistance * YDistance);
    _Robot->GetWantToCloseWBID() = _WorkBenchVec[MinID]->GetWorkBenchID();
    _Robot->GetRobotWorkBenchDis() = RealDis;
    _Robot->GetAngleDifference() = AngleDifference;
    double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
    // cerr << "AngleSpeed" << AngleSpeed << endl;
    // cerr << "enenene " << endl;
    return AngleSpeed * flag;
}
double Move(Robot* _Robot, int MinID, vector<Workbench*> &_WorkBenchVec){
        int flag = 0;    
        vector<double> _axis = _Robot->GetAxis();
        double XDistance =  _WorkBenchVec[MinID]->GetAxis()[0] - _axis[0];
        double YDistance =  _WorkBenchVec[MinID]->GetAxis()[1] - _axis[1];
        int quadrant = 0;
        if(YDistance > 0.0 && XDistance > 0.0)
            quadrant = 1;
        else if(YDistance < 0.0 && XDistance > 0.0)
            quadrant = 4;
        else if(YDistance > 0.0 && XDistance < 0.0)
            quadrant = 2;
        else
            quadrant = 3;

        double Angle = atan2(YDistance, XDistance);
        // cerr << "Angle" << Angle << endl;
        double AngleDifference = abs(Angle - _Robot->GetTowards());
        if(abs(AngleDifference) < 0.5)
            flag = Angle - _Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
        else    
            flag = 1;
        double RealDis =  sqrt(XDistance * XDistance + YDistance * YDistance);
        _Robot->GetWantToCloseWBID() = _WorkBenchVec[MinID]->GetWorkBenchID();
        _Robot->GetRobotWorkBenchDis() = RealDis;
        _Robot->GetAngleDifference() = AngleDifference;
        double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
        // cerr << "AngleSpeed" << AngleSpeed << endl;
        // cerr << "enenene " << endl;
        return AngleSpeed * flag;
}
double Buy(Robot* _Robot, vector<Workbench*> & _WorkBenchVec){
    // cerr << (void*)&_WorkBenchVec << endl;
    vector<int> ID;
    ID = FindVectorWBMinID(_Robot, _WorkBenchVec);
    _Robot -> HaveTarget = ID[1];
    _WorkBenchVec.erase(_WorkBenchVec.begin() + ID[0]);
    // cerr << "ID" << ID[0] << endl;
    // cerr << "LEN: " << _WorkBenchVec.size() << endl;
    return Move(_Robot, ID[1], WorkBenchVec);
}
double Sell(Robot* _Robot, vector<vector<Workbench*>> &_WorkBenchLossSelf){
    vector<int> ID;
    switch(_Robot->TypeArticleCarry){
        case 1:{
            ID = FindVectorWBMinIDSell(_Robot, _WorkBenchLossSelf[4], 1);
            _Robot -> HaveTarget = ID[1]; 
            if(ID[1] == -1)
                break;
            _WorkBenchLossSelf[4].erase(_WorkBenchLossSelf[4].begin() + ID[0]);
            return Move(_Robot, ID[1], WorkBenchVec);
        }
        case 2:{
             ID = FindVectorWBMinIDSell(_Robot, _WorkBenchLossSelf[6], 2);
            _Robot -> HaveTarget = ID[1];
            if(ID[1] == -1)
                break;
            _WorkBenchLossSelf[6].erase(_WorkBenchLossSelf[6].begin() + ID[0]);
            return Move(_Robot, ID[1], WorkBenchVec);           
        }
        case 3:{
             ID = FindVectorWBMinIDSell(_Robot, _WorkBenchLossSelf[5], 3);
            _Robot -> HaveTarget = ID[1]; 
            if(ID[1] == -1)
                break;
            _WorkBenchLossSelf[5].erase(_WorkBenchLossSelf[5].begin() + ID[0]);
            return Move(_Robot, ID[1], WorkBenchVec);
        }
    }
    return 0.0;
}
int main() {
    // vector<double> ve1(2,1.0);
    // vector<double> ve2(2,2.0);
    // cout << ve1 - ve2 <<endl;
    MapInit();
    puts("OK");
    fflush(stdout);
    int frameID;
    int WorkBenchNum;
    int i;
    const int Ordernum = 4;
    vector<int> Order0 = {1, 4, 2, 4};
    vector<int> Order1 = {1, 5, 3, 5};
    vector<int> Order2 = {2, 6, 3, 6};
    vector<int> Order3 = {2, 6, 3, 6};
    vector<int> Cur(4,0);
    vector<double> AngleSpeed(4, 0.0);
    vector<vector<int>>Order;
    Order.push_back(Order0);
    Order.push_back(Order1);
    Order.push_back(Order2);
    Order.push_back(Order3);
    vector<double> LineSpeed(4,0.0);
    while (scanf("%d", &frameID) != EOF) {
        cin >> Money;
        cin >> WorkBenchNum;
        // cerr << "frameID " << frameID << " Money " << Money <<" WorkBenchNum " << WorkBenchNum << endl;
        ReadInfo(WorkBenchNum, WorkBenchVec, RobotVec);
        vector<Workbench*> WBProductReady;
        vector<Workbench*> WBProductLoss;
        vector<vector<Workbench*>> WorkBenchLossSelf(10);
        for(i = 0; i < WorkBenchNum; i++){//判断工作台产品格状态， 放入不同队列
            if(WorkBenchVec[i] -> ProductStatus == 0)
                WBProductLoss.push_back(WorkBenchVec[i]);
            else
                WBProductReady.push_back(WorkBenchVec[i]);
        }
        if(!WBProductLoss.empty()){
            WorkBenchAllocate(WBProductLoss, WorkBenchLossSelf);            
            // for(i = 1 ; i < 10; i++){
            //     cerr << "Workbench" << i << "'s number is " << WorkBenchLossSelf[i].size() << endl;
            // }
        }
        printf("%d\n", frameID);
        // cerr << (void*)&WBProductReady << endl;
        for(i = 0; i < 4; i++){//判断机器人当前是否持有产品
            if(RobotVec[i] -> TypeArticleCarry == 0 && RobotVec[i] -> HaveTarget == -1 && WBProductReady.size()!=0){
                AngleSpeed[i] = Buy(RobotVec[i], WBProductReady);
                cerr << "if1" << endl;
            }
            else if(RobotVec[i] -> TypeArticleCarry == 0 && RobotVec[i] -> HaveTarget != -1){
                cerr << "if2" << endl;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i] -> HaveTarget, WorkBenchVec);
            }
            else if(RobotVec[i] -> TypeArticleCarry != 0 && RobotVec[i] -> HaveTarget == -1){
                cerr << "if3" << endl;
                AngleSpeed[i] = Sell(RobotVec[i], WorkBenchLossSelf);
            }else if(RobotVec[i] -> TypeArticleCarry != 0 && RobotVec[i] -> HaveTarget != -1){
                cerr << "if4" << endl;
                AngleSpeed[i] = Move(RobotVec[i], RobotVec[i] -> HaveTarget, WorkBenchVec);
            }
            if(RobotVec[i] -> TypeArticleCarry != 0 && RobotVec[i] -> HaveTarget == -1){
                // RobotVec[i] -> HaveTarget = -1;
                cerr << "if5" << endl;
                cout << "destroy " << i << endl;
                exit(1);
            }
        }
        for(i = 0; i < 4; i++){
            cerr << i<<"target " << RobotVec[i] -> HaveTarget << endl;
        }




        // for(i = 0; i < WorkBenchSelf[1].size(); i++){
        //    WorkBenchSelf[1][i]->GetAxis(); 
        // }
        // for (i = 0; i < WorkBenchVec.size(); i++){
        //     if(WorkBenchVec[i]->ProductStatus==1){
        //         if(WorkBenchVec[i]->WorkBenchKind == 4){
        //             Qfour.push(WorkBenchVec[i]->WorkBenchID);
        //         }
        //         if(WorkBenchVec[i]->WorkBenchKind == 5){
        //             Qfive.push(WorkBenchVec[i]->WorkBenchID);
        //         }
        //         if(WorkBenchVec[i]->WorkBenchKind == 6){
        //             Qsix.push(WorkBenchVec[i]->WorkBenchID);
        //         }
        //     }
        // }
        // int UrgentId;
        // int NearlestRobotId;
        // if(!Qsix.empty()){
        //     UrgentId = Qsix.front();
        //     Qsix.pop();
        //     NearlestRobotId = FindMinDistance(RobotVec, WorkBenchVec[UrgentId]);
        //     if(RobotVec[NearlestRobotId] -> ProductFlag == 0){
        //         RobotVec[NearlestRobotId] -> UrgentWork = UrgentId;
        //     }
        // }
        // cerr << "Qsix:" << UrgentId << endl;        
        // for(i = 0; i < 4; i++){
        //     if(RobotVec[i]->ProductFlag == 6){
        //         AngleSpeed[i] =  CalculateDistance(RobotVec[i], WorkBenchSelf[7]);
        //     }else{
        //         if(RobotVec[i]->UrgentWork == -1){
        //             RobotVec[i]->WantTOCloseWBKind = Order[i][Cur[i]];
        //             AngleSpeed[i] =  CalculateDistance(RobotVec[i], WorkBenchSelf[RobotVec[i]->WantTOCloseWBKind]);  
        //         }
        //         else{
        //             AngleSpeed[i] = Move(RobotVec[i], RobotVec[i] -> UrgentWork, WorkBenchVec);
        //         }
        //     }
        // }
        // cerr << "teststetstes" << endl;
        for(i = 0; i < 4; i++){
            if(abs(AngleSpeed[i]) < 0.2)
                LineSpeed[i] = 3;
            else
                LineSpeed[i] = 0.5;
        }
        // cerr << "Speed" <<LineSpeed[0]<< endl;
        for(int robotId = 0; robotId < 4; robotId++){
            int FlagBuy = 0;
            int FlagSell = 0;
            printf("forward %d %lf\n", robotId, LineSpeed[robotId]);
            // cerr << "robotId " << robotId << " Linespeed : " <<   LineSpeed[robotId] << endl;
            printf("rotate %d %f\n", robotId, AngleSpeed[robotId]);
            // cerr << "robotId " << robotId << " AngleSpeed : " <<  AngleSpeed[robotId] << endl;
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() && WorkBenchVec[RobotVec[robotId]->WorkBenchID]->Role == 1){
                cout << "sell " << robotId << endl;
                fflush(stdout);
                //Cur[robotId] = (++Cur[robotId] % 4);
                // RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = 0;
                RobotVec[robotId] -> HaveTarget = -1;
                RobotVec[robotId] -> ProductFlag = 0;
                RobotVec[robotId] -> UrgentWork = -1;
                FlagSell = 1;
            }
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() 
            && (WorkBenchVec[RobotVec[robotId]->WorkBenchID]->Role != 2) ){
                // cur0++;
                //Cur[robotId] = (++Cur[robotId] % 4);
                // RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = 0;
                if(WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()] -> ProductStatus == 1){
                    cout << "buy " << robotId << endl;
                    fflush(stdout);
                    RobotVec[robotId] -> HaveTarget = -1;
                    RobotVec[robotId] -> ProductFlag = WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()] -> WorkBenchKind;
                }
                cerr << "product : " << RobotVec[robotId] -> ProductFlag << endl;
                FlagBuy = 1;
                // printf("buy %d\n", 1);
                // exit(1);
            }
            if(FlagBuy||FlagSell){
                if(RobotVec[robotId] -> ProductFlag != 6){
                    Cur[robotId] = (++Cur[robotId] % 4);
                    RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                }
            }  
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    WorkBenchMemRelease(WorkBenchVec);
    RobotMemRelease(RobotVec);
    return 0;
}
