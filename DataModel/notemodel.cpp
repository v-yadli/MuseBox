#include "notemodel.h"
#include <QHash>
#include <QByteArray>
#include <QDebug>
#include <QObject>
#include "../hardware.h"

NoteModel::NoteModel(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "notename";
    roles[PatternRole] = "notepattern";
    roles[PaddingRole] = "notepadding";
    roles[OffsetRole] = "noteoffset";
    roles[LengthRole] = "notelength";
    roles[PositionRole] = "noteposition";
    setRoleNames(roles);

    connect((const QObject*)&Hardware::messageBus,SIGNAL(patternDeleted(Pattern*)),(const QObject*)this,SLOT(patternDeleted(Pattern*)));
    PatternNote dummyNote;
    dummyNote.pattern = NULL;
    dummyNote.length = 0;
    dummyNote.position = -800;//So nobody can truncate this one, or remove this one.
    dummyNote.offset = 0;

    noteList.insert(0,dummyNote);
}

int NoteModel::rowCount(const QModelIndex& index) const
{
    return noteList.count();
}

QVariant NoteModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() > noteList.count())
        return QVariant();

    int r = index.row();
    switch(role)
    {
    case PositionRole:
        return noteList[r].position;
    case NameRole:
        if(noteList[r].pattern != 0){
            return noteList[r].pattern->name;
        }else return "???";
    case PatternRole:return toString(noteList[r].pattern);
    case PaddingRole:
        if(r == noteList.count() - 1)//no padding
            return 0;
        else
            return noteList[r+1].position - noteList[r].position - noteList[r].length;
    case OffsetRole:return noteList[r].offset;
    case LengthRole:return noteList[r].length;
    default:return QVariant();
    }
}

bool NoteModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() > noteList.count())
        return false;

    return false;//TODO currently mostly only passive. Alter data with arrangement interfaces.
}

void NoteModel::patternDeleted(Pattern *p)
{
    //TODO
}

StereoData NoteModel::query(int position)
{
    StereoData s;
    s.l = s.r = 0;
    if(position >= 0){
        foreach(PatternNote p, noteList)
        {
            if(p.position <= position && p.position + p.length >=position)
            {
                int offset = position - p.position + p.offset;
                offset %= p.pattern->length();
                s.l = p.pattern->Get(0,offset);
                s.r = p.pattern->Get(1,offset);
                return s;
            }
        }
    }
    return s;

}

void NoteModel::insertNote(QString patternToken, int bar, int beat)
{
    insertNote(patternToken, Hardware::TransposeMachine->CalculatePosition(bar,beat));
}
void NoteModel::insertNote(QString patternToken, int position)
{
    HWLOCK;
    Pattern* ptr = (Pattern*)fromString(patternToken);
    if(ptr == 0)
    {
        qDebug()<<"NULL ptr in insertNote()!";
        return;
    }
    PatternNote note;
    note.length = ptr->length();
    note.offset = 0;
    note.pattern = ptr;

    insertNote(note, position);
}
void NoteModel::insertNote(PatternNote& note, int position)
{
    if(position >=0)
        note.position = position;
    else{
        if(note.length + position > 0){
            note.position = 0;
            note.offset = (note.offset - position) % note.pattern->length();
            if(note.offset < 0)
                note.offset += note.pattern->length();
            note.length += position;
        }else{
            qDebug()<<"insertNote: fully truncating the note(because it passes zero), and it gets deleted.";
            return;
        }
    }
    int i;
    for(i=0;i<noteList.count();++i)
    {
        if(noteList[i].position >= position)
        {
            beginInsertRows(QModelIndex(),i,i);
            noteList.insert(i,note);
            endInsertRows();
            //Altered is the padding information for previous one, notify the view. And possibly truncate the previous.
            QModelIndex idx = index(i-1);
            emit dataChanged(idx,idx);
            //If overlapping with next one, move the head of next one.
            if(position + note.length > noteList[i+1].position)
            {
                moveNoteHead(i+1, position + note.length - noteList[i+1].position);//possibly deleting that node
            }
            return;
        }
    }
    //The last element
    beginInsertRows(QModelIndex(),i,i);
    noteList.insert(i,note);
    endInsertRows();
    QModelIndex idx = index(i-1);
    emit dataChanged(idx,idx);
}

void NoteModel::removeNote(QString patternToken, int position)
{
    HWLOCK;
    Pattern* ptr = (Pattern*)fromString(patternToken);
    if(ptr == 0)
    {
        qDebug()<<"NULL ptr in removeNote()!";
        return;
    }
    for(int i=1;i<noteList.count();++i)//Don't touch the dummy note!
    {
        if(noteList[i].position == position)
        {
            if(noteList[i].pattern != ptr)
            {
                qDebug()<<"Cannot deleting the note with same position but different pattern!(Why?)";
                return;
            }
            removeNote(i);
            return;
        }
    }
    qDebug()<<"Cannot find desired pattern note to delete! why???";
}
void NoteModel::removeNote(int index)
{
    beginRemoveRows(QModelIndex(),index,index);
    noteList.removeAt(index);
    endRemoveRows();
    //Altered is the padding information for previous one, notify the view.
    QModelIndex idx = this->index(index-1);
    emit dataChanged(idx,idx);
    return;
}

void NoteModel::moveNote(QString patternToken, int oldPosition, int newPosition)
{
    HWLOCK;
    Pattern* ptr = (Pattern*)fromString(patternToken);
    if(ptr == 0)
    {
        qDebug()<<"NULL ptr in moveNote()!";
        return;
    }
    for(int i=1;i<noteList.count();++i)//Don't touch the dummy note!
    {
        if(noteList[i].position == oldPosition)
        {
            if(noteList[i].pattern != ptr)
            {
                qDebug()<<"Cannot move the note with same position but different pattern!(Why?)";
                return;
            }
            beginRemoveRows(QModelIndex(),i,i);
            PatternNote note = noteList.takeAt(i);
            endRemoveRows();
            //Altered is the padding information for previous one, notify the view.
            QModelIndex idx = index(i-1);
            emit dataChanged(idx,idx);
            insertNote(note,newPosition);
            return;
        }
    }
    qDebug()<<"Cannot find desired pattern note to move! why???";
}

void NoteModel::moveNoteHead(QString patternToken, int position, int delta)
{
    HWLOCK;
    Pattern* ptr = (Pattern*)fromString(patternToken);
    if(ptr == 0)
    {
        qDebug()<<"NULL ptr in moveNoteHead()!";
        return;
    }
    for(int i=1;i<noteList.count();++i)//Don't touch the dummy note!
    {
        if(noteList[i].position == position)
        {
            moveNoteHead(i,delta);
            return;
        }
    }
    qDebug()<<"Cannot find desired pattern note to move! why???";
}
void NoteModel::moveNoteHead(int index, int delta)
{
    QModelIndex idx = this->index(index);
    if(delta >=0){//truncating
        if(noteList[index].length > delta){
            noteList[index].length -= delta;
            noteList[index].offset += delta;
            noteList[index].offset %= noteList[index].pattern->length();
            noteList[index].position += delta;
            emit dataChanged(idx,idx);
            //Adjust padding setting of previous one
            QModelIndex prev = this->index(index - 1);
            emit dataChanged(prev,prev);
        }else{
            qDebug()<<"moveNoteHead: note fully truncated. delete.";
            removeNote(index);
        }
    }else{//expanding backward
        if(noteList[index].position + delta < 0)
            delta = -noteList[index].position;
        noteList[index].position += delta;
        noteList[index].length -= delta;
        noteList[index].offset = (noteList[index].offset + delta) % noteList[index].pattern->length();
        if(noteList[index].offset < 0)
            noteList[index].offset += noteList[index].pattern->length();
        emit dataChanged(idx,idx);
        for(int i=index-1;i > 0;--i)//Truncate the previous clips
        {
            if(noteList[i].position + noteList[i].length <= noteList[index].position)
            {
                //Adjust padding settings
                QModelIndex _idx = this->index(i);
                emit dataChanged(_idx,_idx);
                break;
            }
            //Affected!!! Apocalypse Please!!!
            if(noteList[i].position < noteList[index].position){ //Survived!
                noteList[i].length = noteList[index].position - noteList[i].position;
                QModelIndex _idx = this->index(i);
                emit dataChanged(_idx,_idx);
            }else{ //Destroyed!
                removeNote(i);
                --index;//The index of the killer is also moved.
            }
        }
    }
}

void NoteModel::moveNoteTail(QString patternToken, int position, int delta)
{
    HWLOCK;
    Pattern* ptr = (Pattern*)fromString(patternToken);
    if(ptr == 0)
    {
        qDebug()<<"NULL ptr in moveNoteTail()!";
        return;
    }
    for(int i=1;i<noteList.count();++i)//Don't touch the dummy note!
    {
        if(noteList[i].position == position)
        {
            moveNoteTail(i,delta);
            return;
        }
    }
    qDebug()<<"Cannot find desired pattern note to move! why???";
}
void NoteModel::moveNoteTail(int index, int delta)
{
    QModelIndex idx = this->index(index);
    if(delta >= 0){//expanding
        noteList[index].length += delta;
        emit dataChanged(idx,idx);
        for(int i=index+1; i<noteList.count();++i){
            if(noteList[index].position + noteList[index].length <= noteList[i].position)
                break;
            if(noteList[index].position + noteList[index].length <
               noteList[i].position + noteList[i].length){//truncate, then break
                int offset = (noteList[index].position + noteList[index].length - noteList[i].position);
                noteList[i].position += offset;
                noteList[i].offset = (noteList[i].offset + offset) % noteList[i].pattern->length();
                noteList[i].length -= offset;
                QModelIndex _idx = this->index(i);
                emit dataChanged(_idx,_idx);
                break;
            }
            //Delete the note
            removeNote(i);
            --i;
        }
    }else{//truncating
        if(noteList[index].length + delta > 0){
            noteList[index].length += delta;
            emit dataChanged(idx,idx);
        }else{
            qDebug()<<"moveNoteTail: note fully truncated. delete.";
            removeNote(index);
        }
    }
}

void NoteModel::onBPMChange(double oldBPM, double newBPM)
{
    //TODO
    throw "Not implemented";
}
Qt::ItemFlags NoteModel::flags(const QModelIndex &index) const
{
    if(index.row()<0 || index.row() > noteList.count())
        return 0;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}
