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
    return abs(_left[0] - _right[0]) + abs(_left[1] - _right[1]);
}

int FindMinDistanceTargetWB(Robot* _Robot, vector<Workbench*> _WorkBenchVec){
    int i;
    int MinID;
    vector<double> _axis = _Robot->GetAxis();
    double MinDistance = 1e10;
    for(i = 0; i < _WorkBenchVec.size(); i++){
        double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
        if(DistanceTmp < MinDistance){
            MinDistance = DistanceTmp;
            MinID= i;
        }
    }
    return MinID;
}

double CalculateDistance(Robot* _Robot, vector<Workbench*> _WorkBenchVec){
    int i;
    int MinID;
    vector<double> _axis = _Robot->GetAxis();
    // double MinDistance = 1e10;
    // // cerr << "WorkBenchVec.size()" << _WorkBenchVec.size() << endl;
    // for(i = 0; i < _WorkBenchVec.size(); i++){
    //     double DistanceTmp = _axis - _WorkBenchVec[i]->GetAxis();
    //     if(DistanceTmp < MinDistance){
    //         MinDistance = DistanceTmp;
    //         MinID= i;
    //     }
    // }
    // if(_Robot->HaveTargetWBFlag == 0){
    MinID = FindMinDistanceTargetWB(_Robot, _WorkBenchVec);
        // _Robot->HaveTargetWBFlag = 1;
    // }
    int flag = 0;
    double XDistance =  _WorkBenchVec[MinID]->GetAxis()[0] - _axis[0];
    double YDistance =  _WorkBenchVec[MinID]->GetAxis()[1] - _axis[1];
    double Angle = atan2(YDistance, XDistance);
    double AngleDifference = abs(Angle - _Robot->GetTowards());
    // cerr << AngleDifference << endl;
    flag = Angle - _Robot->GetTowards() > 0.0 ? 1 : -1; // 1:ni -1 sun;
    double RealDis =  sqrt(XDistance * XDistance + YDistance * YDistance);
    _Robot->GetWantToCloseWBID() = _WorkBenchVec[MinID]->GetWorkBenchID();
    _Robot->GetRobotWorkBenchDis() = RealDis;
    _Robot->GetAngleDifference() = AngleDifference;
    double AngleSpeed = AngleDifference > 1.5 ? 1.5 : AngleDifference;
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
    while (scanf("%d", &frameID) != EOF) {
        cin >> Money;
        cin >> WorkBenchNum;
        // cerr << "frameID " << frameID << " Money " << Money <<" WorkBenchNum " << WorkBenchNum << endl;
        ReadInfo(WorkBenchNum, WorkBenchVec, RobotVec);

        double MaxDistance = 0;
        for(i = 0; i < WorkBenchSelf[1].size(); i++){
           WorkBenchSelf[1][i]->GetAxis(); 
        }
        double angleSpeed =  CalculateDistance(RobotVec[0], WorkBenchSelf[1]);
        printf("%d\n", frameID);
        int lineSpeed = 1;
        for(int robotId = 0; robotId <1; robotId++){
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
            if(RobotVec[0]->GetWorkBenchID() == RobotVec[0]->GetWantToCloseWBID()){
                cout << "buy " << 0 << endl;
                fflush(stdout);
                // printf("buy %d\n", 1);
                // exit(1);
            }
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    WorkBenchMemRelease(WorkBenchVec);
    RobotMemRelease(RobotVec);
    return 0;
}
