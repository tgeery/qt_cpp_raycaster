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

class Pick : public QObject
{
public:
    Pick(Qt3DRender::QScreenRayCaster * r) : ray(r) {}

public slots:
    void rayHit(const Qt3DRender::QAbstractRayCaster::Hits hits) {
        qDebug() << "rayHit";
        for(auto h : hits) {
            qDebug() << h.entity() << " " << h.vertex1Index() << "," << h.vertex2Index() << "," << h.vertex3Index();
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

    auto torus = new Qt3DCore::QEntity(root);
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
    material->setDiffuse(Qt::red);
    torus->addComponent(mesh);
    torus->addComponent(material);

    auto camera = window->camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setViewCenter(QVector3D(0.f,0.f,0.f));
    camera->setPosition(QVector3D(0.f,0.f,10.f));
    camera->setNearPlane(0.01f);
    camera->setFarPlane(100000.f);

    auto camController = new Qt3DExtras::QOrbitCameraController(root);
    camController->setCamera(camera);

    auto ray = new Qt3DRender::QScreenRayCaster(root);
    ray->setRunMode(Qt3DRender::QAbstractRayCaster::SingleShot);
    auto pick = new Pick(ray);

    QObject::connect(ray, &Qt3DRender::QScreenRayCaster::hitsChanged, pick, &Pick::rayHit);

    window->setRootEntity(root);
    window->show();

    return a.exec();
}
