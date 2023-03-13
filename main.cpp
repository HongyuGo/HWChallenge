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
int ss;
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

int main() {
    MapInit();
    puts("OK");
    fflush(stdout);
    int frameID;
    int WorkBenchNum;
    while (scanf("%d", &frameID) != EOF) {
        cin >> Money;
        cin >> WorkBenchNum;
        cerr << "frameID " << frameID << " Money " << Money <<" WorkBenchNum " << WorkBenchNum << endl;
        ReadInfo(WorkBenchNum, WorkBenchVec, RobotVec);



        
        printf("%d\n", frameID);
        int lineSpeed = 3;
        double angleSpeed = 1.5;
        for(int robotId = 0; robotId <1; robotId++){
            printf("forward %d %d\n", robotId, lineSpeed);
            printf("rotate %d %f\n", robotId, angleSpeed);
        }
        printf("OK\n", frameID);
        fflush(stdout);
    }
    WorkBenchMemRelease(WorkBenchVec);
    RobotMemRelease(RobotVec);
    return 0;
}
