/** \file
 *  Game Develop
 *  2008-2013 Florian Rival (Florian.Rival@gmail.com)
 */

#include "SpriteTools.h"
#include <string>
#include <vector>
#include "GDL/SpriteObject.h"
#include "GDL/RuntimeScene.h"
#include "GDL/Collisions.h"
#include "GDL/profile.h"

using namespace std;

bool GD_API SpriteTurnedToward( std::map <std::string, std::vector<RuntimeObject*> *> objectsLists1, std::map <std::string, std::vector<RuntimeObject*> *> objectsLists2, float tolerance, bool conditionInverted )
{
    bool sameObjectLists = objectsLists1.size() == objectsLists2.size();
    if ( sameObjectLists ) //Make sure that objects lists are really the same
    {
        for (std::map <std::string, std::vector<RuntimeObject*> *>::iterator it1 = objectsLists1.begin(), it2 = objectsLists2.begin();
             it1 != objectsLists1.end() && it2 != objectsLists2.end();
             ++it1, ++it2)
        {
            if ( it1->second != it2->second )
            {
                sameObjectLists = false;
                break;
            }
        }
    }

    bool isTrue = false;

    vector<RuntimeObject*> objects1;
    for (std::map <std::string, std::vector<RuntimeObject*> *>::const_iterator it = objectsLists1.begin();it!=objectsLists1.end();++it)
    {
        if ( it->second != NULL )
        {
            objects1.reserve(objects1.size()+it->second->size());
            std::copy(it->second->begin(), it->second->end(), std::back_inserter(objects1));
            it->second->clear();
        }
    }

    vector<RuntimeObject*> objects2;
    if (sameObjectLists)
        objects2 = objects1;
    else
    {
        for (std::map <std::string, std::vector<RuntimeObject*> *>::const_iterator it = objectsLists2.begin();it!=objectsLists2.end();++it)
        {
            if ( it->second != NULL )
            {
                objects2.reserve(objects2.size()+it->second->size());
                std::copy(it->second->begin(), it->second->end(), std::back_inserter(objects2));
                it->second->clear();
            }
        }
    }

    //Test each object against each other objects
    for ( unsigned int i = 0; i<objects1.size();++i)
    {
        for (unsigned int j = (!sameObjectLists ? 0 : i+1);j<objects2.size();++j)
        {
            //Get angle between objects
            float X = ( objects2[j]->GetDrawableX() + objects2[j]->GetCenterX() ) - ( objects1[i]->GetDrawableX() + objects1[i]->GetCenterX() );
            float Y = ( objects2[j]->GetDrawableY() + objects2[j]->GetCenterY() ) - ( objects1[i]->GetDrawableY() + objects1[i]->GetCenterY() );
            float angle = atan2(Y,X)*180/3.14159;

            float objectAngle = static_cast<RuntimeSpriteObject*>(objects1[i])->GetAngle();

            angle = fmodf(angle, 360);
            if ( angle < 0 ) angle += 360;

            objectAngle = fmodf(objectAngle, 360);
            if ( objectAngle < 0 ) objectAngle += 360;

            float gap = fabs( angle - objectAngle );
            gap = gap > 180 ? 360 - gap : gap;

            if ( gap <= tolerance / 2 )
            {
                if ( !conditionInverted )
                {
                    isTrue = true;
                    if ( find(objectsLists1[objects1[i]->GetName()]->begin(), objectsLists1[objects1[i]->GetName()]->end(), objects1[i]) == objectsLists1[objects1[i]->GetName()]->end() )
                        objectsLists1[objects1[i]->GetName()]->push_back(objects1[i]);

                    if ( find(objectsLists2[objects2[j]->GetName()]->begin(), objectsLists2[objects2[j]->GetName()]->end(), objects2[j]) == objectsLists2[objects2[j]->GetName()]->end() )
                        objectsLists2[objects2[j]->GetName()]->push_back(objects2[j]);
                }
            }
            else
            {
                if ( conditionInverted )
                {
                    isTrue = true;
                    if ( find(objectsLists1[objects1[i]->GetName()]->begin(), objectsLists1[objects1[i]->GetName()]->end(), objects1[i]) == objectsLists1[objects1[i]->GetName()]->end() )
                        objectsLists1[objects1[i]->GetName()]->push_back(objects1[i]);

                    if ( find(objectsLists2[objects2[j]->GetName()]->begin(), objectsLists2[objects2[j]->GetName()]->end(), objects2[j]) == objectsLists2[objects2[j]->GetName()]->end() )
                        objectsLists2[objects2[j]->GetName()]->push_back(objects2[j]);
                }
            }
        }
    }

    return isTrue;
}

/**
 * Test a collision between two sprites objects
 */
bool GD_API SpriteCollision( std::map <std::string, std::vector<RuntimeObject*> *> objectsLists1, std::map <std::string, std::vector<RuntimeObject*> *> objectsLists2, bool conditionInverted )
{
    bool sameLists = objectsLists1.size() == objectsLists2.size();
    if ( sameLists ) //Make sure that objects lists are really the same
    {
        for (std::map <std::string, std::vector<RuntimeObject*> *>::iterator it1 = objectsLists1.begin(), it2 = objectsLists2.begin();
             it1 != objectsLists1.end() && it2 != objectsLists2.end();
             ++it1, ++it2)
        {
            if ( it1->second != it2->second )
            {
                sameLists = false;
                break;
            }
        }
    }

    vector<RuntimeObject*> objects1;
    for (std::map <std::string, std::vector<RuntimeObject*> *>::const_iterator it = objectsLists1.begin();it!=objectsLists1.end();++it)
    {
        if ( it->second != NULL )
        {
            objects1.reserve(objects1.size()+it->second->size());
            std::copy(it->second->begin(), it->second->end(), std::back_inserter(objects1));
            it->second->clear();
        }
    }

    bool isTrue = false;

    if ( !sameLists )
    {
        vector<RuntimeObject*> objects2;
        for (std::map <std::string, std::vector<RuntimeObject*> *>::const_iterator it = objectsLists2.begin();it!=objectsLists2.end();++it)
        {
            if ( it->second != NULL )
            {
                objects2.reserve(objects2.size()+it->second->size());
                std::copy(it->second->begin(), it->second->end(), std::back_inserter(objects2));
                it->second->clear();
            }
        }


        std::vector<RuntimeObject*>::const_iterator obj_end = objects1.end();
        std::vector<RuntimeObject*>::const_iterator obj2_end = objects2.end();

        //On teste la collision entre chaque objets
        for ( std::vector<RuntimeObject*>::const_iterator obj = objects1.begin(); obj != obj_end; ++obj )
        {
            bool collideWithAtLeastOneObject = false;
            for (std::vector<RuntimeObject*>::const_iterator obj2 = objects2.begin(); obj2 != obj2_end; ++obj2 )
            {
                //On v�rifie que ce n'est pas le m�me objet
                if ( *obj != *obj2 &&
                    CheckCollision( static_cast<RuntimeSpriteObject*>(*obj), static_cast<RuntimeSpriteObject*>(*obj2) ) )
                {
                    if ( !conditionInverted )
                    {
                        isTrue = true;

                        //Pick the objects
                        std::vector<RuntimeObject*> * objList = objectsLists1[(*obj)->GetName()];
                        if ( find(objList->begin(), objList->end(), (*obj)) == objList->end() ) objList->push_back((*obj));

                        objList = objectsLists2[(*obj2)->GetName()];
                        if ( find(objList->begin(), objList->end(), (*obj2)) == objList->end() ) objList->push_back((*obj2));

                    }

                    collideWithAtLeastOneObject = true;
                }
            }

            if ( conditionInverted && !collideWithAtLeastOneObject)
            {
                isTrue = true;

                //We are sure that obj is not already in the list.
                //(As we are iterating over objects1 and only objects1 are added )
                objectsLists1[(*obj)->GetName()]->push_back((*obj));
            }
        }
    }
    else
    {
        //On teste la collision entre chaque objets
        for ( unsigned int i = 0; i<objects1.size();++i)
        {
            bool collideWithAtLeastOneObject = false;
            for (unsigned int j = i+1;j<objects1.size();++j)
            {
                if ( CheckCollision( static_cast<RuntimeSpriteObject*>(objects1[i]), static_cast<RuntimeSpriteObject*>(objects1[j]) ) )
                {
                    if ( !conditionInverted )
                    {
                        isTrue = true;

                        std::vector<RuntimeObject*> * objList = objectsLists1[(objects1[i])->GetName()];
                        if ( find(objList->begin(), objList->end(), (objects1[i])) == objList->end() ) objList->push_back((objects1[i]));

                        objList = objectsLists1[(objects1[j])->GetName()];
                        if ( find(objList->begin(), objList->end(), (objects1[j])) == objList->end() ) objList->push_back((objects1[j]));
                    }

                    collideWithAtLeastOneObject = true;
                }
            }

            if ( conditionInverted && !collideWithAtLeastOneObject)
            {
                isTrue = true;

                //We are sure that objects1[i] is not already in the list.
                //(As we are iterating over objects1 and only objects1 are added )
                objectsLists1[(objects1[i])->GetName()]->push_back((objects1[i]));
            }
        }
    }

    return isTrue;
}

