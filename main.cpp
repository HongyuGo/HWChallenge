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
    return sqrt((_left[0] - _right[0]) * (_left[0] - _right[0]) + (_left[1] - _right[1]) * (_left[1] - _right[1]));
}
ostream& operator<<(ostream& os, const vector<double> _axis){
    os << "_axisX : " << _axis[0] << "_axisY : " << _axis[1];
    return os;
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
        if(DistanceTmp < MinDistance && !(_WorkBenchVec[i]->HaveLock(_Robot->TypeArticleCarry, RobotVec))){
        // if(DistanceTmp < MinDistance && _WorkBenchVec[i] -> RobotScheduled == 0){
            // cerr << "Min test" << endl;
            MinDistance = DistanceTmp;
            MinID= i;
        }
    }
    // cerr << "MinID" << MinID << endl;
    // exit(1);
    if(_WorkBenchVec[MinID] -> RobotScheduled == -1)
        _WorkBenchVec[MinID] -> RobotScheduled = _Robot->RobotID;
    // cerr << "MinID " << MinID << endl;
    // cerr << "TargetWB " << _WorkBenchVec[MinID]->WorkBenchID << endl;
    return MinID;
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
    cerr << "MInID : " << MinID << endl;
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

double Move(Robot* _Robot, int _WorkBenchID, vector<Workbench*> _WorkBenchVec){
    vector<double> _axis = _Robot->GetAxis();
    int flag = 0;
    double XDistance =  _WorkBenchVec[_WorkBenchID]->GetAxis()[0] - _axis[0];
    double YDistance =  _WorkBenchVec[_WorkBenchID]->GetAxis()[1] - _axis[1];
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
    _Robot->GetWantToCloseWBID() = _WorkBenchVec[_WorkBenchID]->GetWorkBenchID();
    _Robot->GetRobotWorkBenchDis() = RealDis;
    _Robot->GetAngleDifference() = AngleDifference;
    double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
    // cerr << "AngleSpeed" << AngleSpeed << endl;
    // cerr << "enenene " << endl;
    return AngleSpeed * flag;
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
        
        for(i = 0; i < WorkBenchSelf[1].size(); i++){
           WorkBenchSelf[1][i]->GetAxis(); 
        }
        for(i = 0; i < 4; i++){
            RobotVec[i]->WantTOCloseWBKind = Order[i][Cur[i]];
            AngleSpeed[i] =  CalculateDistance(RobotVec[i], WorkBenchSelf[RobotVec[i]->WantTOCloseWBKind]);
        }
        // cerr << "teststetstes" << endl;
        printf("%d\n", frameID);
        for(i = 0; i < 4; i++){
            if(abs(AngleSpeed[i]) < 0.2)
                LineSpeed[i] = 3;
            else
                LineSpeed[i] = 0.5;
        }
        cerr << "Speed" <<LineSpeed[0]<< endl;
        for(int robotId = 0; robotId < 4; robotId++){
            printf("forward %d %lf\n", robotId, LineSpeed[robotId]);
            // cerr << "robotId " << robotId << " Linespeed : " <<   LineSpeed[robotId] << endl;
            printf("rotate %d %f\n", robotId, AngleSpeed[robotId]);
            // cerr << "robotId " << robotId << " AngleSpeed : " <<  AngleSpeed[robotId] << endl;
            int BuySellFlag = 0;
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() && RobotVec[robotId]->TypeArticleCarry != 0){
                cout << "sell " << robotId << endl;
                fflush(stdout);
                RobotVec[robotId] -> TypeArticleCarry = 0;
                // Cur[robotId] = (++Cur[robotId] % 4);
                // RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                // WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = -1;
                // RobotVec[robotId] -> HaveTarget = -1;
                BuySellFlag = 1;
            }
            if(RobotVec[robotId]->GetWorkBenchID() == RobotVec[robotId]->GetWantToCloseWBID() && RobotVec[robotId]->TypeArticleCarry == 0){
                if(WorkBenchVec[RobotVec[robotId]->GetWorkBenchID()]->ProductStatus == 1){
                    cout << "buy " << robotId << endl;
                    fflush(stdout);
                    BuySellFlag = 1;
                }
                // Cur[robotId] = (++Cur[robotId] % 4);
                // RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                // WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = -1;
                // RobotVec[robotId] -> HaveTarget = -1;
            }
            if(BuySellFlag == 1){
                Cur[robotId] = (++Cur[robotId] % 4);
                RobotVec[robotId]->WantTOCloseWBKind = Order[robotId][Cur[robotId]];
                WorkBenchVec[RobotVec[robotId]->WorkBenchID] -> RobotScheduled = -1;
                RobotVec[robotId] -> HaveTarget = -1;
            }
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    WorkBenchMemRelease(WorkBenchVec);
    RobotMemRelease(RobotVec);
    return 0;
}
