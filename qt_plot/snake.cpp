#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include <QtWidgets/QWidget>

#include <cstdlib>

const int FieldWidth = 20;
const int FieldHeight = 20;
const int SquareSize = 20;
const int GameSpeed = 200;  // milliseconds

class SnakeGame : public QWidget {
 public:
  SnakeGame(QWidget* parent = nullptr) : QWidget(parent) {
    setFixedSize(FieldWidth * SquareSize, FieldHeight * SquareSize);
    setWindowTitle("Snake Game");

    snake = new QLabel(this);
    food = new QLabel(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SnakeGame::move);
    timer->start(GameSpeed);

    resetGame();
  }

 protected:
  void keyPressEvent(QKeyEvent* event) override {
    switch (event->key()) {
      case Qt::Key_Left:
        if (direction != Qt::Key_Right)
          direction = Qt::Key_Left;
        break;
      case Qt::Key_Right:
        if (direction != Qt::Key_Left)
          direction = Qt::Key_Right;
        break;
      case Qt::Key_Up:
        if (direction != Qt::Key_Down)
          direction = Qt::Key_Up;
        break;
      case Qt::Key_Down:
        if (direction != Qt::Key_Up)
          direction = Qt::Key_Down;
        break;
      default:
        QWidget::keyPressEvent(event);
    }
  }

  void move() {
    int x = snake->x();
    int y = snake->y();

    switch (direction) {
      case Qt::Key_Left:
        x -= SquareSize;
        break;
      case Qt::Key_Right:
        x += SquareSize;
        break;
      case Qt::Key_Up:
        y -= SquareSize;
        break;
      case Qt::Key_Down:
        y += SquareSize;
        break;
      default:
        break;
    }

    if (x < 0 || x >= FieldWidth * SquareSize || y < 0 ||
        y >= FieldHeight * SquareSize || checkCollision(x, y)) {
      gameOver();
      return;
    }

    if (x == food->x() && y == food->y()) {
      score += 10;
      food->move(generateRandomCoordinate(), generateRandomCoordinate());
      growSnake();
    } else {
      snakeTrail.removeLast();
      snakeTrail.prepend(QPoint(x, y));
    }

    update();
  }

  void paintEvent(QPaintEvent*) override {
    QPainter painter(this);

    // Draw the snake
    painter.setBrush(Qt::darkGreen);
    for (const QPoint& point : snakeTrail) {
      painter.drawRect(point.x(), point.y(), SquareSize, SquareSize);
    }

    // Draw the food
    painter.setBrush(Qt::red);
    painter.drawRect(food->x(), food->y(), SquareSize, SquareSize);

    // Draw the score
    painter.drawText(rect(), Qt::AlignRight, "Score: " + QString::number(score));
  }

  void growSnake() {
    int x = snakeTrail.first().x();
    int y = snakeTrail.first().y();

    switch (direction) {
      case Qt::Key_Left:
        x += SquareSize;
        break;
      case Qt::Key_Right:
        x -= SquareSize;
        break;
      case Qt::Key_Up:
        y += SquareSize;
        break;
      case Qt::Key_Down:
        y -= SquareSize;
        break;
      default:
        break;
    }

    snakeTrail.prepend(QPoint(x, y));
  }

  bool checkCollision(int x, int y) const {
    for (const QPoint& point : snakeTrail) {
      if (point.x() == x && point.y() == y)
        return true;
    }
    return false;
  }

  int generateRandomCoordinate() const {
    return (rand() % FieldWidth) * SquareSize;
  }

  void resetGame() {
    direction = Qt::Key_Right;
    score = 0;
    snakeTrail.clear();
    snakeTrail.append(QPoint(SquareSize, 0));
    snakeTrail.append(QPoint(0, 0));

    food->move(generateRandomCoordinate(), generateRandomCoordinate());
  }

  void gameOver() {
    timer->stop();

    QString message = "Game Over!\nYour score: " + QString::number(score);
    QMessageBox::information(this, "Snake Game", message);

    resetGame();

    timer->start(GameSpeed);
  }

 private:
  QList<QPoint> snakeTrail;
  QLabel* snake;
  QLabel* food;
  QTimer* timer;
  int direction;
  int score;
};

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  SnakeGame game;
  game.show();

  return app.exec();
}
