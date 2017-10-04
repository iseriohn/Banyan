/**
 * Banyan, an automatic dual-track meeting scheduler
 * 
 * @author Yan Ji
 * @version 1.0 Sep 23, 2017
 */

#include <iostream>
#include <cstdio>
#include <cstring>  
#include <string>
#include <cstdlib>
#include <cmath>
#include <cctype>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <cassert>
#include <bitset>
#include <sstream>
#include <fstream>

using namespace std;

const int INF = 1e9;
const int ITER = 1e3;
const int BITER = 10;
const int SWITCH = 10;
int DELTA = 1;
int SESS = 1;
string OUTPUT = "result.csv";

struct Paper {
    string id;
    string name;
    vector<int> review;
    int session, track;

    Paper(string id = "", string name = "") : id(id), name(name) {
        review.clear();
        session = -1; 
    }
};

struct Reviewer {
    string id;
    string name;
    vector<int> review;
    vector<int> track;

    Reviewer(string id = "", string name = "") : id(id), name(name) {
        review.clear();
        track.clear();
    }
};

struct Track {
    vector<int> paper;
    vector<int> reviewer;

    Track() {
        paper.clear();
        reviewer.clear();
    }
};

struct Session {
    Track tracks[2];

    void clear() {
        for (int i = 0; i < 2; ++i) {
            tracks[i].paper.clear();
            tracks[i].reviewer.clear();
        }
    }
};

vector<Paper> papers;
vector<Reviewer> reviewers;
vector<Session> sessions;

set<pair<int, int> > collisions;

map<string, int> paperProfile;
map<string, int> reviewerProfile;

void init() {
    int num_reviewer = 0, num_paper = 0;

    //ifstream infile("review.in");
    
    string line;
    getline(cin, line);
    
    while (getline(cin, line)) {
        if (line.empty()) continue;
        line += " ";

        string paper_id = "", paper_name = "";
        string reviewer_id = "", reviewer_name = "";
        string tmp = "";

        istringstream iss(line);
        getline(iss, paper_id, ',');

        while (getline(iss, tmp, ',')) {
            paper_name += reviewer_id;  // in case a paper title consists a comma
            reviewer_id = reviewer_name;
            reviewer_name = tmp;
        }

        //cerr << paper_id << ", " << paper_name << ", " << reviewer_id << ", " << reviewer_name << endl;

        if (paperProfile.find(paper_id) == paperProfile.end()) {
            paperProfile[paper_id] = num_paper++;
            papers.push_back(Paper(paper_id, paper_name));
        }
        if (reviewerProfile.find(reviewer_id) == reviewerProfile.end()) {
            reviewerProfile[reviewer_id] = num_reviewer++;
            reviewers.push_back(Reviewer(reviewer_id, reviewer_name));
        }

        papers[paperProfile[paper_id]].review.push_back(reviewerProfile[reviewer_id]);
        reviewers[reviewerProfile[reviewer_id]].review.push_back(paperProfile[paper_id]);
    }

    for (int i = 0; i < num_reviewer; ++i) {
        for (int j = 0; j < reviewers[i].review.size(); ++j) {
            for (int k = 0; k < reviewers[i].review.size(); ++k) {
                int x = reviewers[i].review[j], y = reviewers[i].review[k];
                if (x != y && collisions.find(make_pair(x, y)) == collisions.end()) {
                    collisions.insert(make_pair(x, y));
                    collisions.insert(make_pair(y, x));
                }
            }
        }
    }

    cerr << "#reviewer: " << num_reviewer << ", #paper: " << num_paper <<endl;
}

void test(int num_paper, int num_reviewer) {
    int session = sessions.size();

    for (int i = 0; i < num_paper; ++i) {
        assert(papers[i].session != -1);
    }

    for (int i = 0; i < num_reviewer; ++i) {
        assert(reviewers[i].track.size() == session);
    }

    for (int i = 0; i < session; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < sessions[i].tracks[j].paper.size(); ++k) {
                assert(papers[sessions[i].tracks[j].paper[k]].session == i &&
                        papers[sessions[i].tracks[j].paper[k]].track == j);
            }

            for (int k = 0; k < sessions[i].tracks[j].reviewer.size(); ++k) {
                assert(reviewers[sessions[i].tracks[j].reviewer[k]].track[i] == j);
            }
        
            for (int k = 0; k < sessions[i].tracks[j].paper.size(); ++k) {
                for (int l = 0; l < papers[sessions[i].tracks[j].paper[k]].review.size(); ++l) {
                    assert(reviewers[papers[sessions[i].tracks[j].paper[k]].review[l]].track[i] == j);
                }
            }

        }

    }

    //cerr << "Test passed!" << endl;
}

void updateResult(int session, int num_paper, int num_reviewer) {
    for (int i = 0; i < num_reviewer; ++i) {
        reviewers[i].track.clear();
    }
   
    for (int i = 0; i < sessions.size(); ++i) {
        sessions[i].clear();
    }
    sessions.clear();
   
    for (int i = 0; i < session; ++i) {
        Session singleSess;
        int tot = num_reviewer / 2;

        for (int j = 0; j < num_paper; ++j) {
            if (papers[j].session == i) {
                singleSess.tracks[papers[j].track].paper.push_back(j);
                
                for (int k = 0; k < papers[j].review.size(); ++k) {
                    if (reviewers[papers[j].review[k]].track.size() <= i) {
                        reviewers[papers[j].review[k]].track.push_back(papers[j].track);
                        if (papers[j].track == 0) --tot;
                    }
                }
            }
        }
        
        for (int j = 0; j < num_reviewer; ++j) {
            if (reviewers[j].track.size() <= i) {
                if (tot > 0) {
                    reviewers[j].track.push_back(0);
                    --tot;
                } else {
                    reviewers[j].track.push_back(1);
                }
            }
            singleSess.tracks[reviewers[j].track[i]].reviewer.push_back(j);
        }

        sessions.push_back(singleSess);
    }
}

int balance(int session, int num_paper, int num_reviewer) {
    int** num = new int*[session];
    for (int i = 0; i < session; ++i) {
        num[i] = new int[2];
    }

    int* pos = new int[session];
    int* tos = new int[session];

    for (int iter = 0; iter < BITER; ++iter) {
        for (int i = 0; i < session; ++i) {
            for (int j = 0; j < 2; ++j) {
                num[i][j] = 0;
            }
        }

        for (int i = 0; i < num_paper; ++i) {
            ++num[papers[i].session][papers[i].track];
        }
        
        for (int i = 0; i < session; ++i) {
            if (num[i][0] > num[i][1]) {
                for (int j = 0; j < num_paper; ++j) {
                    if (papers[j].session == i) {
                        papers[j].track = 1 - papers[j].track;
                    }
                }
                swap(num[i][0], num[i][1]);
            }
        }

        for (int i = 0; i < session; ++i) pos[i] = i;
        
        for (int i = 0; i < session; ++i) {
            for (int j = i + 1; j < session; ++j) {
                if (num[pos[i]][0] < num[pos[j]][0]) {
                    swap(pos[i], pos[j]);
                }
            }
        }

        for (int i = 0; i < session; ++i) tos[pos[i]] = i;

        for (int i = 0; i < session; ++i) {
            for (int j = 0; j < 2; ++j) {
                num[i][j] = 0;
            }
        }

        for (int i = 0; i < num_paper; ++i) {
            papers[i].session = tos[papers[i].session];
            ++num[papers[i].session][papers[i].track];
        }

        for (int i = 0; i < (session - 1) / 2; ++i) {
            vector<int> evict[2][2];
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    evict[j][k].clear();
                }
            }

            for (int j = 0; j < num_paper; ++j) {
                if (papers[j].session == i) {
                    int check[2];
                    memset(check, 0, sizeof(check));

                    for (int k = 0; k < num_paper; ++k) {
                        if (papers[k].session == session - 1 - i &&
                                collisions.find(make_pair(j, k)) != collisions.end()) {
                            check[papers[k].track] = 1;
                        }
                    }
                    
                    for (int k = 0; k < 2; ++k) {
                        if (!check[k]) evict[papers[j].track][k].push_back(j);
                    }
                }
            }

            int x = min(evict[0][0].size(), evict[1][1].size());
            int y = min(evict[0][1].size(), evict[1][0].size());

            if (x < y) {
                int z = min(y, (num[i][0] - num[session - 1 - i][0]) / 2);
                for (int j = 0; j < z; ++j) {
                    papers[evict[0][1][j]].session = session - 1 - i;
                    papers[evict[1][0][j]].session = session - 1 - i;
                }
            } else {
                int z = min(x, (num[i][0] - num[session - 1 - i][0]) / 2);
                for (int j = 0; j < z; ++j) {
                    papers[evict[0][0][j]].session = session - 1 - i;
                    papers[evict[0][0][j]].track = 1;
                    papers[evict[1][1][j]].session = session - 1 - i;
                    papers[evict[1][1][j]].track = 0;
                }
            }
        }
    }

    int maxNum = 0;
    
    for (int i = 0; i < session; ++i) {
        for (int j = 0; j < 2; ++j) {
            num[i][j] = 0;
        }
    }

    for (int i = 0; i < num_paper; ++i) {
        ++num[papers[i].session][papers[i].track];
        maxNum = max(maxNum, num[papers[i].session][papers[i].track]);
    }

    for (int i = 0; i < session; ++i) {
        delete[] num[i];
    }
    delete[] num;

    delete[] pos;
    delete[] tos;

    return maxNum;
}

bool schedule() {
    bool ans = false;
    int num_paper = papers.size(), num_reviewer = reviewers.size();
    int minSess = INF, minDel = INF;

    for (int iter = 0; iter < ITER; ++iter) {
        int tot = 0;

        for (int i = 0; i < num_paper; ++i) {
            papers[i].session = -1;
        }

        for (int sess = 0; sess < minSess; ++sess) {
            int track[2] = {0, 0};
            int switchNum = 0;

            int tmp = rand() % (num_paper - tot), insIdx = -1;
            for (int i = 0; i <= tmp; ++i) {
                ++insIdx;
                while (papers[insIdx].session != -1) ++insIdx;
            }

            papers[insIdx].session = sess, papers[insIdx].track = 0;
            ++track[0];

            for (int i = 0; i < num_paper; ++i) {
                if (papers[i].session == -1 && 
                        collisions.find(make_pair(insIdx, i)) == collisions.end()) {
                    papers[i].session = sess, papers[i].track = 1;
                    ++track[1];
                }
            }

            while (abs(track[1] - track[0]) > DELTA) {
                int max = 0, idx = -1;

                int dir = 0;
                if (track[1] < track[0]) {
                    dir = 1;
                    ++switchNum;
                }

                for (int i = 0; i < num_paper; ++i) {
                    if (papers[i].session == -1) {
                        int count = 0;
                        for (int j = 0; j < num_paper; ++j) {
                            if (papers[j].session == sess && papers[j].track == 1 - dir &&
                                    collisions.find(make_pair(i, j)) == collisions.end()) {
                                ++count;
                            }
                        }

                        if (count > max) {
                            max = count;
                            idx = i;
                        }
                    }
                }

                if (idx != -1 && switchNum < SWITCH) {
                    papers[idx].session = sess, papers[idx].track = dir;
                    ++track[dir];

                    for (int i = 0; i < num_paper; ++i) {
                        if (papers[i].session == sess && papers[i].track == 1 - dir && 
                                collisions.find(make_pair(idx, i)) != collisions.end()) {
                            papers[i].session = -1;
                            --track[1 - dir];
                        }
                    }
                } else {
                    for (int i = 0; i < 2; ++i) {
                        if (track[i] - track[1 - i] > DELTA) {
                            int tmp = rand() % track[i], delIdx = -1;
                            for (int j = 0; j <= tmp; ++j) {
                                ++delIdx;
                                while (papers[delIdx].session != sess || papers[delIdx].track != i) {
                                    ++delIdx;
                                }
                            }
                            papers[delIdx].session = -1;
                            --track[i];
                        }
                    }
                }
            }
            tot += track[0] + track[1];
            if (tot == num_paper) {
                if (sess + 1 <= minSess) {
                    sess = max(SESS - 1, sess);
                    int delta = balance(sess + 1, num_paper, num_reviewer);
                    if (sess + 1 < minSess || (sess + 1 == minSess && delta < minDel)) {
                        minSess = max(SESS, sess + 1);
                        minDel = delta;
                        updateResult(minSess, num_paper, num_reviewer);
                        test(num_paper, num_reviewer);
                        ans = true;
                        
                        cerr << "Find a schedule with session: " << minSess << ", maximum paper in a track: " << minDel << endl;
                    }
                }
                break;
            }
        }
    }
    
    return ans;
}

void getResult(int num_paper, int num_reviewer) {
    int session = sessions.size();
    
    for (int i = 0; i < num_paper; ++i) {
        papers[i].session = -1;
    }

    for (int i = 0; i < num_reviewer; ++i) {
        reviewers[i].track.clear();
    }

    for (int i = 0; i < session; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < sessions[i].tracks[j].paper.size(); ++k) {
                papers[sessions[i].tracks[j].paper[k]].session = i;
                papers[sessions[i].tracks[j].paper[k]].track = j;
            }

            for (int k = 0; k < sessions[i].tracks[j].reviewer.size(); ++k) {
                if (reviewers[sessions[i].tracks[j].reviewer[k]].track.size() <= i) {
                    reviewers[sessions[i].tracks[j].reviewer[k]].track.push_back(j);
                }
            }
        }
    }
}

void output() {
    int num_paper = papers.size(), num_reviewer = reviewers.size();
    
    getResult(num_paper, num_reviewer);
    test(num_paper, num_reviewer);

    freopen(OUTPUT.c_str(), "w", stdout);
    
    for (int i = 0; i < sessions.size(); ++i) {
        printf("Assignment for Session #%d\r", i + 1);

        int x = sessions[i].tracks[0].paper.size();
        int y = sessions[i].tracks[1].paper.size();
        printf("Papers in Track #1, %d, Papers in Track #2, %d\r", x, y);
        printf("Paper ID, Title, Paper ID, Title\r");
        for (int j = 0; j < min(x, y); ++j) {
            int u = sessions[i].tracks[0].paper[j];
            int v = sessions[i].tracks[1].paper[j];
            printf("%s, %s, %s, %s\r",
                    papers[u].id.c_str(),
                    papers[u].name.c_str(),
                    papers[v].id.c_str(),
                    papers[v].name.c_str());
        }
        if (x < y) {
            for (int j = x; j < y; ++j) {
                int u = sessions[i].tracks[1].paper[j];
                printf(", , %s, %s\r",
                        papers[u].id.c_str(),
                        papers[u].name.c_str());
            }
        } else if (x > y) {
            for (int j = y; j < x; ++j) {
                int u = sessions[i].tracks[0].paper[j];
                printf("%s, %s, , \r",
                        papers[u].id.c_str(),
                        papers[u].name.c_str());
            }
        }
        printf("\r");

        x = sessions[i].tracks[0].reviewer.size();
        y = sessions[i].tracks[1].reviewer.size();
        printf("Reviewers in Track #1, %d, Reviewers in Track #2, %d\r", x, y);
        printf("Reviewer Name, Email, Reviewer Name, Email\r");
        for (int j = 0; j < min(x, y); ++j) {
            int u = sessions[i].tracks[0].reviewer[j];
            int v = sessions[i].tracks[1].reviewer[j];
            printf("%s, %s, %s, %s\r", 
                    reviewers[u].name.c_str(),
                    reviewers[u].id.c_str(),
                    reviewers[v].name.c_str(),
                    reviewers[v].id.c_str());
        }
        if (x < y) {
            for (int j = x; j < y; ++j) {
                int u = sessions[i].tracks[1].reviewer[j];
                printf(", , %s, %s\r",
                        reviewers[u].name.c_str(),
                        reviewers[u].id.c_str());
            }
        } else if (x > y) {
            for (int j = y; j < x; ++j) {
                int u = sessions[i].tracks[0].reviewer[j];
                printf("%s, %s, , \r",
                        reviewers[u].name.c_str(),
                        reviewers[u].id.c_str());
            }
        }
        printf("\r\r");
    }

    fclose(stdout);

}

int main(int argc, char* argv[]) {
    srand(time(0));

    for (int i = 0; i < argc; ++i) {
        string st = string(argv[i]);
        if (st.find("-output") != string::npos) {
            ++i;
            OUTPUT = string(argv[i]);
            continue;
        }

        if (st.find("-maxdiff") != string::npos) {
            ++i;
            DELTA = atoi(argv[i]);
            continue;
        }

        if (st.find("-minsess") != string::npos) {
            ++i;
            SESS = atoi(argv[i]);
            continue;
        }
    }

    //cerr << OUTPUT << ", " << DELTA << ", " << SESS << endl;

    collisions.clear();
    paperProfile.clear();
    reviewerProfile.clear();

    init();
    
    cerr << "Scheduling, please wait..." << endl;
    
    if (schedule()) {
        output();
        cerr << "Scheduling finished! Please check the assignment in " << OUTPUT << endl;
    } else {
        cerr << "No valid assignment found. Please modify the parameters or contact Yan." << endl;
    }
    
    return 0;
}

