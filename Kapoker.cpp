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
    for (int a = 0; a < 7; a++)
        for (int b = a+1; b < 7; b++) {
            vector<Card> five;
            for (int i = 0; i < 7; i++)
                if (i != a && i != b)
                    five.push_back(h[i]);
            best = max(best, eval5(five));
        }
    return best;
}

bool bettingRound(int &money, int &pot, const vector<Card>& player) {
    char act;

    cout << "Money: $" << money << endl;
    cout << "Pot:   $" << pot << endl;
    cout << "Your cards: ";
    print(player);

    cout << "[C]all (10)  [R]aise  [F]old: ";
    cin >> act;

    if (act == 'F' || act == 'f')
        return false;

    int bet = 10;

    if (act == 'R' || act == 'r') {
        cout << "Raise amount: ";
        cin >> bet;
    }

    if (bet > money) bet = money;

    money -= bet;
    pot += bet * 2;

    return true;
}

void playRound(int &money) {

    int pot = 0;

    vector<Card> deck = makeDeck();
    shuffle(deck.begin(), deck.end(), mt19937(random_device()()));

    vector<Card> player = {deck[0], deck[1]};
    vector<Card> dealer = {deck[2], deck[3]};
    vector<Card> board  = {deck[4], deck[5], deck[6], deck[7], deck[8]};

    cout << "\n=== NEW ROUND ===\n";
    cout << "Your cards: ";
    print(player);

    cout << "\n--- PRE-FLOP ---\n";
    if (!bettingRound(money, pot, player)) return;

    cout << "\n--- FLOP ---\n";
    print({board[0], board[1], board[2]});
    if (!bettingRound(money, pot, player)) return;

    cout << "\n--- TURN ---\n";
    print({board[0], board[1], board[2], board[3]});
    if (!bettingRound(money, pot, player)) return;

    cout << "\n--- RIVER ---\n";
    print(board);
    if (!bettingRound(money, pot, player)) return;

    vector<Card> p7 = player;
    vector<Card> d7 = dealer;

    p7.insert(p7.end(), board.begin(), board.end());
    d7.insert(d7.end(), board.begin(), board.end());

    int ps = eval7(p7);
    int ds = eval7(d7);

    cout << "\n--- SHOWDOWN ---\n";
    cout << "Dealer cards: ";
    print(dealer);

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

int main() {

    int money = 100;
    char again = 'y';

    cout << "=== TEXAS HOLD'EM ===\n";

    while ((again == 'y' || again == 'Y') && money > 0) {

        playRound(money);

        cout << "\nCurrent money: $" << money << endl;
        cout << "Play again? (y/n): ";
        cin >> again;
    }

    cout << "\nGame Over. Final money: $" << money << endl;
    return 0;
}