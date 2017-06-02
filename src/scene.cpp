#include "scene.h"

#include <QAction>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QMimeData>
#include <QUrl>
#include <QWidget>
#include <QDebug>

Scene::Scene() {
  setBackgroundBrush(Qt::gray);
  this->installEventFilter(this);
  sceneMode = NoMode;
  itemToDraw = 0;
  m_rubberBand = 0;
  m_image = 0;
  m_init = false;
}

void Scene::setImage(QPixmap pixmap) {
  m_image = this->addPixmap(pixmap);
}

void Scene::removeImage() {
  this->removeItem(static_cast<QGraphicsItem*>(m_image));
  delete m_image;
  m_image = 0;
}

void Scene::imageInfo() {
  emit imageInfoTriggered();
}

void Scene::rotateCW() {
  emit rotateImage(1);
}

void Scene::rotateCCW() {
  emit rotateImage(3);
}

void Scene::rotateHalf() {
  emit rotateImage(2);
}

void Scene::detectOrientation() {
  emit detectOrientationSignal();
}

void Scene::dragEnterEvent(QGraphicsSceneDragDropEvent * event) {
  event->acceptProposedAction();
}

void Scene::dragLeaveEvent(QGraphicsSceneDragDropEvent * event) {
  event->acceptProposedAction();
}

void Scene::dragMoveEvent(QGraphicsSceneDragDropEvent * event) {
  event->acceptProposedAction();
}

void Scene::dropEvent(QGraphicsSceneDragDropEvent * event) {
  QList<QUrl> urls = event->mimeData()->urls();
  if (urls.count()) {
    QString filename = urls[0].toLocalFile();
    emit dropedFilename(filename);
    event->acceptProposedAction();
  }
}

bool Scene::eventFilter(QObject* object, QEvent* event) {
  if (event->type() == QEvent::GraphicsSceneWheel) {
    QGraphicsSceneWheelEvent *wheelEvent =
      static_cast<QGraphicsSceneWheelEvent*>(event);
    if (wheelEvent->modifiers().testFlag(Qt::ControlModifier)) {
      int delta = wheelEvent->delta();
      qreal scale = 1.00;
      if (delta > 0) {
        scale = 1.10;
      } else {
        scale = 1/1.10;
      }
      emit sceneScaleChanged(scale);
      wheelEvent->accept();
      return true;
    }
  }

  return false;
}

void Scene::contextMenuEvent(QGraphicsSceneContextMenuEvent * event) {
  event->accept();
  QMenu* menu = new QMenu();
  QAction *ImageInfoAction = menu->addAction(QIcon(":/info.svg"),
                             tr("Image info"));
  connect(ImageInfoAction, SIGNAL(triggered()), this, SLOT(imageInfo()));
  menu->addSeparator();

  QAction* rotateCWAction = menu->addAction(QIcon(":/rotateCW.svg"),
                            tr("Rotate CW"));
  connect(rotateCWAction, SIGNAL(triggered()), this, SLOT(rotateCW()));

  QAction* rotateCCWAction = menu->addAction(QIcon(":/rotateCCW.svg"),
                             tr("Rotate CCW"));
  connect(rotateCCWAction, SIGNAL(triggered()), this, SLOT(rotateCCW()));

  QAction* rotateHalfAction = menu->addAction(QIcon(":/rotateCCW.svg"),
                              tr("Rotate 180°"));
  connect(rotateHalfAction, SIGNAL(triggered()), this, SLOT(rotateHalf()));
  QAction* detectOrientationAction = menu->addAction(QIcon(":/orientation.svg"),
                                                     tr("Detect orientation"));
  detectOrientationAction->setToolTip(
    tr("Page orientation detection (four 90 degree angles)"));
  connect(detectOrientationAction, SIGNAL(triggered()), this,
          SLOT(detectOrientation()));
  menu->exec(event->screenPos());
  menu->deleteLater();
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if (event->modifiers() == Qt::ControlModifier &&
                event->button() == Qt::LeftButton) {
        origPoint = event->scenePos();
    }
    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event){
    if(!m_rubberBand && m_image) {
        m_rubberBand = new AreaItem(m_image);
        m_rubberBand->setAreaRect(QRectF(origPoint, event->scenePos()));
        this->addItem(m_rubberBand);
        m_init = true;
    }
    if(m_init) {
        m_rubberBand->setAreaRect(QRectF(origPoint, event->scenePos()));
    }
     QGraphicsScene::mouseMoveEvent(event);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    m_init = false;
    QGraphicsScene::mouseReleaseEvent(event);
}

void Scene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    event->setAccepted(false);
    if (m_rubberBand) {
      delete(m_rubberBand);
      m_rubberBand = 0;
    }
    if (!event->isAccepted())
      QGraphicsScene::mouseDoubleClickEvent(event);
}

void Scene::keyPressEvent(QKeyEvent *event){
    // Not working on m_rubberBand
    if(event->key() == Qt::Key_Delete)
        foreach(QGraphicsItem* item, selectedItems()){
            removeItem(item);
            delete item;
        }
    else
        QGraphicsScene::keyPressEvent(event);
}
