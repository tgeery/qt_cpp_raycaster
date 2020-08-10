#include <QApplication>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender>
#include <Qt3DExtras/QTorusMesh>
#include <QCuboidMesh>
#include <QTorusGeometry>
#include <QPhongMaterial>
#include <QOrbitCameraController>
#include <QPickEvent>
#include <QScreenRayCaster>
#include <QSphereMesh>
#include <QObjectPicker>

class Pick : public QObject
{
public:
    Pick(Qt3DRender::QScreenRayCaster * r) : ray(r) {}

public slots:
    void picked(Qt3DRender::QPickEvent * e) {
        qDebug() << "released at " << e->position().x() << "," << e->position().y();
        ray->trigger(QPoint(e->position().x(), e->position().y()));
    }

    void rayHit(const Qt3DRender::QAbstractRayCaster::Hits hits) {
        qDebug() << "rayHit";
        for(auto h : hits) {
            qDebug() << h.entity()->objectName() << " " << h.worldIntersection().x() << "," << h.worldIntersection().y() << "," << h.worldIntersection().z();
        }
    }

private:
    Qt3DRender::QScreenRayCaster * ray;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto window = new Qt3DExtras::Qt3DWindow;

    auto root = new Qt3DCore::QEntity;

    auto cube = new Qt3DCore::QEntity(root);
    cube->setObjectName("CCube");
    auto mesh = new Qt3DExtras::QCuboidMesh;
    mesh->setXExtent(1.f);
    mesh->setYExtent(1.f);
    mesh->setZExtent(1.f);
    for(int i = 0; i < mesh->geometry()->attributes().size(); i++) {
        mesh->geometry()->attributes().at(i)->buffer()->setSyncData(true);
        qDebug() << "attribute " << \
                    mesh->geometry()->attributes().at(i)->objectName() << " " << \
                    mesh->geometry()->attributes().at(i)->vertexBaseType() << " " << \
                    mesh->geometry()->attributes().at(i)->attributeType() << " " << \
                    mesh->geometry()->attributes().at(i)->vertexSize() << " " << \
                    mesh->geometry()->attributes().at(i)->byteOffset() << " " << \
                    mesh->geometry()->attributes().at(i)->byteStride();
    }
    auto material = new Qt3DExtras::QPhongMaterial;
    material->setDiffuse(Qt::blue);
    cube->addComponent(mesh);
    cube->addComponent(material);

    auto dotMesh = new Qt3DExtras::QSphereMesh;
    dotMesh->setRadius(0.02);
    auto dotMaterial = new Qt3DExtras::QPhongMaterial;
    dotMaterial->setDiffuse(Qt::yellow);
    Qt3DCore::QTransform dotTransform[8];
    dotTransform[0].setTranslation(QVector3D(0.5, 0.5, 0.5));
    dotTransform[1].setTranslation(QVector3D(0.5, -0.5, 0.5));
    dotTransform[2].setTranslation(QVector3D(-0.5, -0.5, 0.5));
    dotTransform[3].setTranslation(QVector3D(-0.5, 0.5, 0.5));
    dotTransform[4].setTranslation(QVector3D(0.5, 0.5, -0.5));
    dotTransform[5].setTranslation(QVector3D(0.5, -0.5, -0.5));
    dotTransform[6].setTranslation(QVector3D(0.5, -0.5, -0.5));
    dotTransform[7].setTranslation(QVector3D(-0.5, 0.5, -0.5));
    for(int i = 0; i < 8; i++) {
        auto dot = new Qt3DCore::QEntity(root);
        dot->addComponent(dotMesh);
        dot->addComponent(dotMaterial);
        dot->addComponent(&dotTransform[i]);
    }

    auto camera = window->camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    camera->setPosition(QVector3D(0.0f, 0.0f, 10.0f));
    camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera->setNearPlane(0.1f);
    camera->setFarPlane(1000.f);
    camera->setFieldOfView(45.0f);
    camera->setAspectRatio(16/9);

    auto camController = new Qt3DExtras::QOrbitCameraController(root);
    camController->setCamera(camera);

    auto picker = new Qt3DRender::QObjectPicker(root);
    auto ray = new Qt3DRender::QScreenRayCaster(root);
    ray->setRunMode(Qt3DRender::QAbstractRayCaster::SingleShot);
    auto pick = new Pick(ray);
    QObject::connect(picker, &Qt3DRender::QObjectPicker::released, pick, &Pick::picked);
    QObject::connect(ray, &Qt3DRender::QScreenRayCaster::hitsChanged, pick, &Pick::rayHit);
    root->addComponent(picker);
    root->addComponent(ray);

    window->setRootEntity(root);
    window->show();

    return a.exec();
}
