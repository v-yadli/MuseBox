#include "patternmodel.h"
#include <QHash>
#include "../hardware.h"
#include <QByteArray>


PatternModel::PatternModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[PatternRole] = "pattern";
    roles[SessionRole] = "session";
    setRoleNames(roles);

    nextPatternID = 1;
    connect((const QObject*)&Hardware::messageBus,SIGNAL(patternDeleted(Pattern*)),(const QObject*)this,SLOT(patternDeleted(Pattern*)));
}

PatternModel::~PatternModel()
{
    foreach(Pattern* p, patternList)
    {
        if(p->recordingSession != NULL){
            p->recordingSession = NULL;
            if(this != p->recordingSession)
                delete p->recordingSession;
        }
        delete p;
    }
}

int PatternModel::rowCount(const QModelIndex& index) const
{
    return patternList.count();
}

QVariant PatternModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > patternList.count())
        return QVariant();

    const Pattern* pattern = patternList[index.row()];
    switch(role)
    {
    case NameRole:return pattern->name;
    case PatternRole:return toString(pattern);
    default:return QVariant();
    }
}

bool PatternModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() > patternList.count())
        return false;

    Pattern* pattern = patternList[index.row()];
    switch(role)
    {
    case NameRole:
        pattern->name = value.toString();
        qDebug()<<QString("Changing the name of pattern #%1 to %2").arg(index.row()).arg(pattern->name);
        emit dataChanged(index,index);
        return true;
    case PatternRole://The qml view is not allowed to modify the pointer
        return false;
    default:return false;
    }
}

Pattern* PatternModel::recordNewPattern()
{
    int row = patternList.count();
    QModelIndex parent;
    beginInsertRows(parent,row,row);
    Pattern* pattern = new Pattern();
    int id = nextPatternID++;

    pattern->name = QString("Pattern %1").arg(id);
    pattern->recordingSession = this;
    patternList.insert(row, pattern);
    endInsertRows();
    return pattern;
}

bool PatternModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(count!=1)return false;
    if(row <0||row >=patternList.count())return false;
    beginRemoveRows(parent,row,row+count-1);
    Pattern *pattern = patternList.takeAt(row);

    Hardware::messageBus.deletePattern(pattern);
    delete pattern;

    endRemoveRows();
    return true;
}

void PatternModel::import(PatternModel *m)
{
    beginInsertRows(QModelIndex(),patternList.count(),patternList.count()+m->patternList.count());
    foreach(Pattern* p,m->patternList)
    {
        patternList.push_back(p);
    }
    endInsertRows();
}

Qt::ItemFlags PatternModel::flags(const QModelIndex &index) const
{
    if(index.row()<0 || index.row() > patternList.count())
        return 0;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void PatternModel::patternDeleted(Pattern *p)
{
    int idx = patternList.indexOf(p);
    if(idx >=0 )
    {
        QModelIndex parent;
        beginRemoveRows(parent,idx,idx);
        patternList.takeAt(idx);
        endRemoveRows();
    }
}
