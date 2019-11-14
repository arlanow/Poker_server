#include "room.h"

Room::Room(QString name, QString password, int maxPlayers):name(name),password(password),maxPlayers(maxPlayers)
{
    gameTimer = new QTimer();
    gameTimer->setInterval(30000);
    connect(gameTimer, SIGNAL(timeout()), this, SLOT(timeout()));
}

bool Room::addPlayer(QString username, int money)
{
    if (players.contains(username)) return false;
    if (players.count()<maxPlayers) {
        players.insert(username, new Player(money));
        return true;
    } else return false;
}

QJsonObject Room::getInfo(QString username)
{
    QJsonObject info;
    info.insert("blind", blind);
    info.insert("step", step);
    info.insert("game", game);
    info.insert("started", gameTimer->isActive());
    info.insert("remainingTime", gameTimer->remainingTime());
    QJsonArray gameCards;
    int cardsNum = 0;
    if (step>1) cardsNum = step+1;
    if (step>4) cardsNum = 5;
    for (auto j=0; j<cardsNum; j++) {
        gameCards.append(cards.at(j));
    }
    for (auto j=0; j<5-cardsNum; j++) {
        gameCards.append(0);
    }
    info.insert("cards", gameCards);
    QJsonArray playersInfo;
    for (auto i=0; i<players.count(); i++) {
        auto key = players.keys().at(i);
        auto player = players.value(key);
        QJsonObject playerInfo;
        playerInfo.insert("name", key);
        playerInfo.insert("bet", player->currentBet);
        playerInfo.insert("dealer", dealer == i);
        playerInfo.insert("betTurn", betTurn == i);
        playerInfo.insert("money", player->money);
        QJsonArray playerCards;
        if (step==0) {
            playerCards.append(0);
            playerCards.append(0);
        }
        else if (key==username) {
            playerCards.append(player->cards.first);
            playerCards.append(player->cards.second);
        } else if (step==FinalStep) {
            playerCards.append(player->cards.first);
            playerCards.append(player->cards.second);
        } else {
            playerCards.append(0);
            playerCards.append(0);
        }
        playerInfo.insert("cards", playerCards);
        playersInfo.append(playerInfo);
    }
    info.insert("players", playersInfo);
    return info;
}

void Room::nextTurn()
{
    bool nextTurnIsNecessary = false;
    for (auto i=0; i<players.count(); i++) {
        auto currentPlayerName = players.keys().at(i);
        auto currentPlayer = players.value(currentPlayerName);
        if (currentPlayer->currentBet<getMinimumBet(currentPlayerName)) {
            nextTurnIsNecessary = true;
        }
    }
    if (nextTurnIsNecessary) {
        qDebug() << "nextTurnIsNecessary";
        if (betTurn==players.count()-1) betTurn = 0;
        else betTurn++;
    } else {
        betTurn = 0;
        step++;
        if (step>5) endGame();
    }
    gameTimer->start();
}

void Room::findWinner()
{
    lastWinners.clear();
    QMap<QString, int> scores;
    for (auto playerName:players.keys()) {
        scores.insert(playerName, 0);
    }
    for (auto i=0; i<players.count(); i++) {
        auto playerName = players.keys().at(i);
        auto player = players.value(playerName);
        if (player->cards.first != 0) {
            auto playerCards = cards;
            playerCards.append(player->cards.first);
            playerCards.append(player->cards.second);
            scores.insert(playerName, calcScore(playerCards));
        }
    }
    auto maxScore = 0;
    for (auto i=0; i<scores.count(); i++) {
        auto playerName = scores.keys().at(i);
        if (scores.value(playerName)>maxScore) maxScore = scores.value(playerName);
    }
    for (auto i=0; i<scores.count(); i++) {
        auto playerName = scores.keys().at(i);
        if (scores.value(playerName)==maxScore) lastWinners.append(playerName);
    }
    // Find max card

}

int Room::calcScore(QList<int> cards)
{
    int score = 0;
    int maxPair = 0, maxPair2 = 0, maxSet = 0, maxStraight = 0;
    // Старшая карта
    int maxCard = 0;
    for (auto card:cards) {
        if ((card-1)/4>maxCard) maxCard = (card-1/4);
    }
    score += maxCard;

    // Стрит
    for (auto i=0; i<3; i++) {
        if ((cards.at(i)-1)/4 == (cards.at(i+1)-1)/4-1 == (cards.at(i+2)-1)/4-2 == (cards.at(i+3)-1)/4-3 == (cards.at(i+4)-1)/4-4) {
            if ((cards.at(i+4)-1)/4>maxStraight) maxStraight = (cards.at(i+4)-1)/4;
        }
        if ((cards.at(i)-1)/4 == (cards.at(i+1)-1)/4+1 == (cards.at(i+2)-1)/4+2 == (cards.at(i+3)-1)/4+3 == (cards.at(i+4)-1)/4+4) {
            if ((cards.at(i)-1)/4>maxStraight) maxStraight = (cards.at(i)-1)/4;
        }
    }
    if (maxStraight>0) {
        score+=4000+maxStraight*10;
        return score;
    }

    // Сет
    for (auto i=0; i<cards.count(); i++) {
        for (auto j=i; j<cards.count(); j++) {
            for (auto k=j; k<cards.count(); k++) {
                if ((cards.at(i)-1)/4 == (cards.at(j)-1)/4 == (cards.at(k)-1)/4) {
                    if ((cards.at(i)-1)/4>maxSet) {
                        maxSet = (cards.at(i)-1)/4;
                    }
                }
            }
        }
    }
    if (maxSet>0) {
        score+=3000+maxSet*10;
        return score;
    }

    // Две пары
    for (auto i=0; i<cards.count(); i++) {
        for (auto j=i; j<cards.count(); j++) {
            if ((cards.at(i)-1)/4 == (cards.at(j)-1)/4) {
                if ((cards.at(i)-1)/4>maxPair) {
                    maxPair2 = maxPair;
                    maxPair = (cards.at(i)-1)/4;
                } else if ((cards.at(i)-1)/4>maxPair2) {
                    maxPair2 = (cards.at(i)-1)/4;
                }
            }
        }
    }
    if (maxPair>0 && maxPair2>0) {
        score+=2000+maxPair*10;
        return score;
    }

    // Пара
    for (auto i=0; i<cards.count(); i++) {
        for (auto j=i; j<cards.count(); j++) {
            if ((cards.at(i)-1)/4 == (cards.at(j)-1)/4) {
                if ((cards.at(i)-1)/4>maxPair) maxPair = (cards.at(i)-1)/4;
            }
        }
    }
    if (maxPair>0) {
        score+=1000+maxPair*10;
    }
    return score;
}

void Room::endGame()
{
    auto prizePool = 0;
    for (auto player:players) {
        player->decrementMoney(player->currentBet);
        prizePool += player->currentBet;
        player->currentBet = 0;
    }
    findWinner();
    for (auto winner:lastWinners) {
        players.value(winner)->incrementMoney(prizePool/lastWinners.count());
    }
    gameTimer->stop();
}

bool Room::checkAccess(QString username)
{
    return players.contains(username);
}

int Room::getMinimumBet(QString username)
{
    int maxBet = 0;
    for (auto i=0; i<players.count(); i++) {
        auto key = players.keys().at(i);
        auto player = players.value(key);

        if (key == username) {
            if (dealer == i) {
                if (blind/2>maxBet) maxBet = blind/2;
            } else if ((dealer==maxPlayers-1 && i==0)||(dealer+1==i)) {
                if (blind>maxBet) maxBet = blind;
            }
        }
        if (player->currentBet>maxBet) maxBet = player->currentBet;
    }
    return maxBet;
}

bool Room::bet(QString username, int money)
{
    if (playersTurn(username)) {
        if (getMinimumBet(username)<=money) {
            nextTurn();
            return players.value(username)->bet(money);
        }
    }
    return false;
}

bool Room::fold(QString username)
{
    players.value(username)->cards = QPair<int, int>(0,0);
    if (playersTurn(username)) {
        gameTimer->start();
    }
    return true;
}

bool Room::call(QString username)
{
    return bet(username, getMinimumBet(username));
}

void Room::genCardPool()
{
    cardsPool.clear();
    for (auto i=1; i<=52; i++) {
        cardsPool.append(i);
    }
    int c1, c2;
    for (auto i=0; i<52; i++) {
        c1 = QRandomGenerator::system()->bounded(52);
        c2 = c1;
        while (c2==c1) {
            c2 = QRandomGenerator::system()->bounded(52);
        }
        cardsPool.swapItemsAt(c1, c2);
    }
    for (auto i=0; i<5; i++) {
        cards.append(cardsPool.takeLast());
    }
    for (auto player: players) {
        player->cards.first = cardsPool.takeLast();
        player->cards.second = cardsPool.takeLast();
    }
}

void Room::newGame()
{
    gameTimer->stop();
    for (auto player:players) {
        player->cards = QPair<int,int>(0,0);
    }
    cards.clear();
    genCardPool();
    if (dealer == players.count()-1) dealer = 0; else dealer++;
    betTurn = dealer;
    game++;
}

bool Room::gameStart()
{
    if (!gameTimer->isActive()) {
        newGame();
        gameTimer->start();
        return true;
    }
    return false;
}

bool Room::playersTurn(QString username)
{
    for (auto i=0; i<players.count(); i++) {
        if (players.keys().at(i)==username) {
            if (i==betTurn) return true;
        }
    }
    return false;
}

int Room::leave(QString username)
{
    int canFold = players.value(username)->money - players.value(username)->currentBet;
    freeMoney = players.value(username)->currentBet;
    players.remove(username);
    return canFold;
}

void Room::timeout()
{
    if (step==5) {
        newGame();
    }
    else {
        auto currentPlayerName = players.keys().at(betTurn);
        auto currentPlayer = players.value(currentPlayerName);
        if (currentPlayer->currentBet<getMinimumBet(currentPlayerName)) fold(currentPlayerName);
        nextTurn();
    }
}
