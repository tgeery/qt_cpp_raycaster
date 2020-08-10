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
    Pick(Qt3DRender::QScreenRayCaster * r) : ray(r), prevMat(nullptr) {}

public slots:
    void clicked(Qt3DRender::QPickEvent * e) {
//        qDebug() << "released at " << e->position().x() << "," << e->position().y();
        ray->trigger(QPoint(e->position().x(), e->position().y()));
        if(prevMat)
            prevMat->setDiffuse(Qt::yellow);
    }

    void released(Qt3DRender::QPickEvent * e) {
        qDebug() << "released " << e->position().x() << "," << e->position().y();
    }

    void rayHit(const Qt3DRender::QAbstractRayCaster::Hits hits) {
//        qDebug() << "rayHit";
        for(auto h : hits) {
            QString name = h.entity()->objectName();
            if(name[0] == 'd' && name[1] == 'o' && name[2] == 't') {
                qDebug() << h.entity()->objectName() << " " << h.worldIntersection().x() << "," << h.worldIntersection().y() << "," << h.worldIntersection().z();
                for(auto a : h.entity()->components()) {
                    qDebug() << a;
                    if(a->objectName()[0] == 'm' && a->objectName()[1] == 'a' && a->objectName()[2] == 't') {
//                        qDebug() << "found it";
                        Qt3DExtras::QPhongMaterial * mat = qobject_cast<Qt3DExtras::QPhongMaterial *>(a);
                        if(mat) {
                            mat->setDiffuse(Qt::red);
                            prevMat = mat;
                        } else
                            qDebug() << "Error getting material";
//                        a->setProperty("diffuse", Qt::red);
                    }
                }
            }
        }
    }

private:
    Qt3DRender::QScreenRayCaster * ray;
    Qt3DExtras::QPhongMaterial * prevMat;
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
        dot->setObjectName(QString("dot%0").arg(i));
        auto dotMaterial = new Qt3DExtras::QPhongMaterial;
        dotMaterial->setObjectName(QString("mat%0").arg(i));
        dotMaterial->setDiffuse(Qt::yellow);
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
    picker->setDragEnabled(true);
    auto ray = new Qt3DRender::QScreenRayCaster(root);
    ray->setRunMode(Qt3DRender::QAbstractRayCaster::SingleShot);
    auto pick = new Pick(ray);
//    QObject::connect(picker, &Qt3DRender::QObjectPicker::clicked, pick, &Pick::clicked);
//    QObject::connect(picker, &Qt3DRender::QObjectPicker::released, pick, &Pick::released);
    QObject::connect(picker, &Qt3DRender::QObjectPicker::clicked, pick, &Pick::clicked);
    QObject::connect(ray, &Qt3DRender::QScreenRayCaster::hitsChanged, pick, &Pick::rayHit);
    root->addComponent(picker);
    root->addComponent(ray);

    window->setRootEntity(root);
    window->show();

    return a.exec();
}
