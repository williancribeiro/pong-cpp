#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <list>
#include <time.h>


int generateRandInt(int minNum, int maxNum)
//gera um inteiro entre minNum e maxNum, maxNum > minNum e maxNum > 20 e minNum < -20.
//Se o número estiver entre -20 e 20 irá gerar um novo número
{
    int randInt = 0;
    srand(time(NULL));
    int range = maxNum - minNum +1;
    while(randInt > - 20 && randInt < 20)
    {
        randInt = rand() % range + minNum;
    }

    return randInt;
}

int randOneOrMinusOne()
//retorno aleatório 1 or -1
{
    srand(time(NULL));
    int chance = rand() % 2;
    if(chance == 1)
    {
        return 1;
    }
    else if(chance == 0)
    {
        return -1;
    }
}

void serverConnect(sf::TcpSocket& socket, unsigned int port)
{
    sf::TcpListener listener;
    listener.listen(port);
    if(listener.accept(socket) == sf::Socket::Done)
    {
        std::cout << "connected" << std::endl;
    }
}

void clientConnect(sf::TcpSocket& socket, unsigned int port, std::string ipAdress)
{
    if(socket.connect(ipAdress, port) == sf::Socket::Done)
    {
        std::cout << "connected" << std::endl;
    }
}


class IInputObserver
{
public:
    virtual void updateInput(sf::Event event) = 0;
};

class InputManager
{
private:
    std::list<IInputObserver*> observers_;

    void notify(sf::Event event)
    {
        std::list<IInputObserver*>::const_iterator iter;
        for(iter = observers_.begin(); iter != observers_.end(); iter++)
        {
            (*iter)->updateInput(event);
        }
    }

public:
    void addObserver(IInputObserver* observer)
    {
        observers_.push_back(observer);
    }

    void deleteObserver(IInputObserver* observer)
    {
        observers_.remove(observer);
    }

    void handleInput(sf::Event event, sf::RenderWindow& window)
    {
        if(event.key.code == sf::Keyboard::Escape)
        {
            window.close();
        }
        else
        {
            this->notify(event);
        }

    }
};


class Paddle: public sf::RectangleShape, public IInputObserver
{
public:
    Paddle(float posX, float posY, float speedY, sf::Vector2f rectSize = sf::Vector2f(10,100))
    {
        this->setPosition(posX, posY);
        this->setSize(rectSize);
        speedY_ = speedY;
    }

    void update(double &dt, sf::RenderWindow &window)
    {
        if(moveUp_ == true)
        {
            this->move(0, -speedY_ * dt);
            if(this->getPosition().y < 0)
            {
                this->setPosition(this->getPosition().x, 0.f);
            }
        }
        if(moveDown_ == true)
        {
            this->move(0, speedY_ * dt);
            if(this->getPosition().y > window.getSize().y - this->getSize().y)
            {
                this->setPosition(this->getPosition().x, window.getSize().y - this->getSize().y);
            }
        }
    }

    void updateInput(sf::Event event)
    {
        switch(event.key.code)
        {
        case sf::Keyboard::Up:
            if( event.type == sf::Event::KeyPressed)
            {
                moveUp_ = true;
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                moveUp_ = false;
            }
            break;
        case sf::Keyboard::Down:
            if( event.type == sf::Event::KeyPressed)
            {
                moveDown_ = true;
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                moveDown_ = false;
            }
            break;
        default:
            break;
        }

    }

    void setScore(int newScore)
    {
        score_ = newScore;
    }

    void addScore(int amount)
    {
        score_ += amount;
    }

    int getScore() const
    {
        return score_;
    }

private:
    float speedY_;
    bool moveUp_ = false;
    bool moveDown_ = false;
    int score_ = 0;
};

class Ball : public sf::CircleShape
{
public:
    Ball(float posX, float posY, float speedX, float speedY, float radius=3.f)
    {
        this->setPosition(posX, posY);
        this->setRadius(radius);
        speedX_ = speedX;
        speedY_ = speedY;
    }

    void update(double& dt, sf::RenderWindow& window, Paddle& paddleLeft, Paddle& paddleRight)
    {
        //VERIFIQUE SE A BOLA ATINGE O PADDLE (primeiro verifique a paddle direita, depois o paddle esquerda)
        if(this->getPosition().x < paddleRight.getPosition().x + paddleRight.getSize().x && this->getPosition().x + speedX_ * dt + this->getRadius()*2 > paddleRight.getPosition().x
           && this->getPosition().y < paddleRight.getPosition().y + paddleRight.getSize().y && this->getPosition().y + this->getRadius()*2 > paddleRight.getPosition().y && speedX_ > 0)
        {
            this->setPosition(paddleRight.getPosition().x - this->getRadius()*2, this->getPosition().y);
            this->changeSpeedX();
        }
        else if (this->getPosition().x + this->getRadius()*2 > paddleLeft.getPosition().x && this->getPosition().x + speedX_ *dt < paddleLeft.getPosition().x + paddleLeft.getSize().x
                 && this->getPosition().y < paddleLeft.getPosition().y + paddleLeft.getSize().y && this->getPosition().y + this->getRadius()*2 > paddleLeft.getPosition().y && speedX_ < 0)
        {
            this->setPosition(paddleLeft.getPosition().x+paddleLeft.getSize().x, this->getPosition().y);
            this->changeSpeedX();
        }

        //MOVIMENTAÇÃO DA BOLA
        this->move(speedX_ * dt, speedY_ * dt);

        //VERIFICAÇÂO SE A BOLA ATINGE O TOPO OU A PARTE INFERIOR
        if(this->getPosition().y <= 0 || this->getPosition().y  >= window.getSize().y - this->getRadius()*2)
        {
            speedY_ = -speedY_;
            if(speedY_ < 0 && speedY_ >= -1000)
            {
                this->setPosition(this->getPosition().x, window.getSize().y - this->getRadius()*2);
                speedY_ -= 50;
            }
            else if(speedY_ > 0 && speedY_ <= 1000)
            {
                this->setPosition(this->getPosition().x, 0);
                speedY_ += 50;
            }
        }
    }

    void changeSpeedX()
    {
        speedX_ = -speedX_;
        if(speedX_ < 0 && speedX_ >= -1000)
        {
            speedX_ -= 50;
        }
        else if(speedX_ > 0 && speedX_ <= 1000)
        {
            speedX_ += 50;
        }
    }

    void setSpeed(float newSpeedX, float newSpeedY)
    {
        speedX_ = newSpeedX;
        speedY_ = newSpeedY;
    }

private:
    float speedX_, speedY_;
};

void printScore(Paddle& paddleLeft, Paddle& paddleRight)
{
    std::cout << "JogadorEsquerda: " << paddleLeft.getScore() << " JogadorDireita: " << paddleRight.getScore() << std::endl;
}

class Referee
{
public:
    void checkGoal(sf::RenderWindow& window, Paddle& paddleLeft, Paddle& paddleRight, Ball& ball)
    {
        if(ball.getPosition().x + ball.getRadius()*2 < 0)
        {
            paddleRight.addScore(1);
            std::cout << "JogadorEsquerda: " << paddleLeft.getScore() << " JogadorDireita: " << paddleRight.getScore() << std::endl;
            ball.setPosition(window.getSize().x/2, window.getSize().y/2);
            ball.setSpeed(randOneOrMinusOne()*200,float(generateRandInt(-200,200)));
        }
        else if(ball.getPosition().x > window.getSize().x)
        {
            paddleLeft.addScore(1);
            printScore(paddleLeft, paddleRight);
            ball.setPosition(window.getSize().x/2, window.getSize().y/2);
            ball.setSpeed(randOneOrMinusOne()*200,float(generateRandInt(-200,200)));
        }
    }
private:

};

void serverNetwork(sf::TcpSocket& socket, Paddle& paddleLeft, Paddle& paddleRight, Ball& ball)
{
    sf::Packet packetSend;
    sf::Packet packetReceive;
    sf::Mutex mutex;
    float newPaddleRightX = paddleRight.getPosition().x;
    float newPaddleRightY = paddleRight.getPosition().y;

    mutex.lock();
    packetSend << paddleLeft.getPosition().x << paddleLeft.getPosition().y << paddleLeft.getScore()
               << paddleRight.getScore() << ball.getPosition().x << ball.getPosition().y;
    mutex.unlock();

    socket.send(packetSend);

    socket.receive(packetReceive);
    packetReceive >> newPaddleRightX >> newPaddleRightY;

    if(newPaddleRightX != paddleRight.getPosition().x || newPaddleRightY != paddleRight.getPosition().y)
    {
        paddleRight.setPosition(newPaddleRightX, newPaddleRightY);
    }

}

void clientNetwork(sf::TcpSocket& socket, Paddle& paddleLeft, Paddle& paddleRight, Ball& ball)
{
    sf::Packet packetSend;
    sf::Packet packetReceive;
    sf::Mutex mutex;
    float newPaddleLeftX = paddleLeft.getPosition().x;
    float newPaddleLeftY = paddleLeft.getPosition().y;
    int newPaddleLeftScore = paddleLeft.getScore();
    int newPaddleRightScore = paddleRight.getScore();
    float newBallX = ball.getPosition().x;
    float newBallY = ball.getPosition().y;

    mutex.lock();
    packetSend << paddleRight.getPosition().x << paddleRight.getPosition().y;
    mutex.unlock();

    socket.send(packetSend);

    socket.receive(packetReceive);
    packetReceive >> newPaddleLeftX >> newPaddleLeftY >> newPaddleLeftScore >> newPaddleRightScore
                    >> newBallX >> newBallY;

    if(newBallX != ball.getPosition().x || newBallY != ball.getPosition().y)
    {
        ball.setPosition(newBallX, newBallY);
        paddleLeft.setPosition(newPaddleLeftX, newPaddleLeftY);
        if(newPaddleLeftScore != paddleLeft.getScore() || newPaddleRightScore != paddleRight.getScore())
        {
            paddleLeft.setScore(newPaddleLeftScore);
            paddleRight.setScore(newPaddleRightScore);
            printScore(paddleLeft, paddleRight);
        }

    }
}

int main()
{
    //CRIAR JANELA
    sf::RenderWindow window(sf::VideoMode(800,600), "A1 PingPong");
    window.setFramerateLimit(100);
    window.setKeyRepeatEnabled(false);
    window.setMouseCursorVisible(false);

    //VARIÁVEIS
    const unsigned short PORT = 5320;
    InputManager inputManager;
    Referee referee;
    sf::Event event;
    sf::Clock clock;
    double dt = 0.0;

    //REDE
    sf::TcpSocket socket;
    sf::Packet packetSend;
    sf::Packet packetReceive;
    char networkChoice;

    //PADDLES (float posX, float posY, float speedY, sf::Vector2f rect)
    Paddle paddleLeft(10,window.getSize().y/2 - 50,300,sf::Vector2f(10,100));
    paddleLeft.setFillColor(sf::Color(100,250,50));
    Paddle paddleRight(window.getSize().x - 20, window.getSize().y/2 - 50, 300, sf::Vector2f(10,100));
    paddleRight.setFillColor(sf::Color(250,100,50));

    //BOLA (float posX, float posY, float speedX, float speedY, float radius=3)
    Ball ball(window.getSize().x/2, window.getSize().y/2,randOneOrMinusOne()*200,float(generateRandInt(-200,200)));
    ball.setFillColor(sf::Color::Blue);
    std::cout << ball.getOrigin().x << " " << ball.getOrigin().y;

    sf::CircleShape test(20);
    test.setFillColor(sf::Color::Cyan);
    test.setPosition(200,200);
    sf::CircleShape test2(3);

    //REGISTRAR PADDLES PARA A ENTRADA DO JOGADOR
    //inputManager.addObserver(&paddleRight);
    //inputManager.addObserver(&paddleLeft);

    std::cout << "Entre com Servidor(s) ou Cliente(c):";
    std::cin >> networkChoice;

    if(networkChoice == 's')
    {
        serverConnect(socket, 5002);
        inputManager.addObserver(&paddleLeft);
    }
    else if(networkChoice == 'c')
    {
        std::string ipAdress;
        std::cout << "Entre com o IP do servidor: ";
        std::cin >> ipAdress;
        clientConnect(socket, 5002,ipAdress);
        inputManager.addObserver(&paddleRight);
    }

    //REINICIAR RELÓGIO, PODERÁ SER EXCLUÍDO MAIS TARDE
    clock.restart();

    while(window.isOpen())
    {
        while(window.pollEvent(event))
        {
            switch(event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                inputManager.handleInput(event, window);
                break;
            case sf::Event::KeyReleased:
                inputManager.handleInput(event, window);
                break;
            default:
                break;
            }
        }

        //LÓGICA DO JOGO
        dt = clock.restart().asSeconds();

        if(networkChoice == 's')
        {
            paddleLeft.update(dt, window);
            ball.update(dt,window, paddleLeft, paddleRight);
            serverNetwork(socket, paddleLeft, paddleRight, ball);
            referee.checkGoal(window,paddleLeft, paddleRight, ball);
        }
        else if(networkChoice == 'c')
        {
            paddleRight.update(dt, window);
            clientNetwork(socket, paddleLeft, paddleRight, ball);
        }


        //EMPATE
        window.clear(sf::Color::Black);
        window.draw(paddleLeft);
        window.draw(paddleRight);
        window.draw(ball);
        window.display();

    }
}
