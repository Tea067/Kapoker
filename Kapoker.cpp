#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <string>

using namespace std;

struct Card {
    int rank;
    char suit;
};

string r2s(int r) {
    if (r <= 10) return to_string(r);
    if (r == 11) return "J";
    if (r == 12) return "Q";
    if (r == 13) return "K";
    return "A";
}

void print(const vector<Card>& h) {
    for (auto &c : h)
        cout << r2s(c.rank) << c.suit << " ";
    cout << endl;
}

vector<Card> makeDeck() {
    vector<Card> d;
    char s[] = {'H','D','C','S'};
    for (char x : s)
        for (int r = 2; r <= 14; r++)
            d.push_back({r,x});
    return d;
}

int eval5(vector<Card> h) {
    map<int,int> cnt;
    vector<int> r;

    for (auto &c : h) {
        cnt[c.rank]++;
        r.push_back(c.rank);
    }

    bool flush = true;
    for (int i = 1; i < 5; i++)
        if (h[i].suit != h[0].suit) flush = false;

    sort(r.begin(), r.end());

    bool straight = true;
    for (int i = 0; i < 4; i++)
        if (r[i] + 1 != r[i+1]) straight = false;

    if (flush && straight && r[4] == 14) return 10;
    if (flush && straight) return 9;
    for (auto &p : cnt) if (p.second == 4) return 8;
    if (cnt.size() == 2) return 7;
    for (auto &p : cnt) if (p.second == 3) return 6;
    if (cnt.size() == 3) return 5;
    if (straight) return 4;
    if (flush) return 3;
    if (cnt.size() == 4) return 2;
    return 1;
}

int eval7(vector<Card> h) {
    int best = 0;

    int n = h.size();

    for (int a = 0; a < n; a++)
        for (int b = a+1; b < n; b++) {

            vector<Card> five;

            for (int i = 0; i < n; i++)
                if (i != a && i != b)
                    five.push_back(h[i]);

            if (five.size() == 5)
                best = max(best, eval5(five));
        }

    return best;
}

bool bettingRound(int &money, int &pot, const vector<Card>& player) {

    char act;

    cout << "----------------------\n";
    cout << "Current money: $" << money << endl;
    cout << "Current pot:   $" << pot << endl;

    cout << "Your cards: ";
    print(player);

    cout << "[C]all (10)  [R]aise  [F]old: ";
    cin >> act;

    if (act == 'F' || act == 'f') {
        cout << "You folded.\n";
        return false;
    }

    int bet = 10;

    if (act == 'R' || act == 'r') {
        cout << "Raise amount: ";
        cin >> bet;

        if (bet > money)
            bet = money;
    }

    money -= bet;
    pot += bet * 2;

    cout << "Dealer calls.\n";
    cout << "Updated pot: $" << pot << endl;
    cout << "----------------------\n";

    return true;
}

void chaosEvent(int &pot, vector<Card>& board, vector<Card>& player, vector<Card>& dealer, vector<Card>& deck) {

    int chaos = rand() % 4;

    cout << "\n===== CHAOS EVENT =====\n";

    if (chaos == 0) {
        cout << "POT DOUBLED!\n";
        pot *= 2;
    }

    else if (chaos == 1) {

        cout << "EXTRA COMMUNITY CARD!\n";

        board.push_back(deck[9]);

        cout << "Board: ";
        print(board);
    }

    else if (chaos == 2) {

        cout << "LAST BOARD CARD REMOVED!\n";

        if (!board.empty())
            board.pop_back();

        cout << "Board: ";
        print(board);
    }

    else {

        cout << "PLAYER AND DEALER SWAP FIRST CARD!\n";

        swap(player[0], dealer[0]);
    }

    cout << "=======================\n";
}

int main() {

    srand(time(0));

    int money = 100;

    char again = 'y';

    cout << "=== CHAOS HOLD'EM ===\n";

    while (again == 'y' || again == 'Y') {

        if (money <= 0) {
            cout << "You are out of money!\n";
            break;
        }

        int pot = 0;

        vector<Card> d = makeDeck();

        shuffle(d.begin(), d.end(), mt19937(random_device()()));

        vector<Card> player = {d[0], d[1]};
        vector<Card> dealer = {d[2], d[3]};
        vector<Card> board  = {d[4], d[5], d[6], d[7], d[8]};

        cout << "\nYour money: $" << money << endl;
        cout << "Your cards: ";
        print(player);

        cout << "\n--- PRE-FLOP ---\n";
        if (!bettingRound(money, pot, player)) goto endRound;

        cout << "\n--- FLOP ---\n";
        print({board[0], board[1], board[2]});
        if (!bettingRound(money, pot, player)) goto endRound;

        cout << "\n--- TURN ---\n";
        print({board[0], board[1], board[2], board[3]});
        if (!bettingRound(money, pot, player)) goto endRound;

        cout << "\n--- RIVER ---\n";
        print(board);
        if (!bettingRound(money, pot, player)) goto endRound;

        chaosEvent(pot, board, player, dealer, d);

        {
            vector<Card> p7 = player;
            vector<Card> d7 = dealer;

            p7.insert(p7.end(), board.begin(), board.end());
            d7.insert(d7.end(), board.begin(), board.end());

            int ps = eval7(p7);
            int ds = eval7(d7);

            cout << "\n--- SHOWDOWN ---\n";

            cout << "Dealer cards: ";
            print(dealer);
            cout << "Your cards: ";
            print(player);

            if (ps > ds) {
                money += pot;
                cout << "YOU WIN $" << pot << endl;
            }

            else if (ps < ds) {
                cout << "DEALER WINS\n";
            }

            else {
                money += pot / 2;
                cout << "DRAW\n";
            }
        }

    endRound:

        cout << "Current money: $" << money << endl;

        cout << "\nPlay again? (y/n): ";
        cin >> again;
    }

    cout << "\nGame over. Final money: $" << money << endl;

    return 0;
}