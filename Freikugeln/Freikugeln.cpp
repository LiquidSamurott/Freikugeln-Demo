#include <iostream>
#include <Windows.h>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <ctime>
#include <vector>
#include <cstdlib>
using namespace std;
//for the gameplay loop
bool weapontaken = false;
bool characterSelected = false;
bool professionSelected = false;
bool actionExecuted = false;
bool gameEnded = false;
bool searchedDesk = false;
bool searchedPainting = false;
bool searchedCollection = false;
bool searchedBookshelves = false;
bool satisfied = false;
bool intelobtained = false;
bool bombobtained = false;
//character creation
char playername[256];
char response[3];
char profession[7];
string selectedprofession, location;
int titlescreen, choice;
int str = 0;
int per = 0;
int knw = 0;
int chr = 0;
int agi = 0;
int ste = 0;

struct weapons {
    string name = "None";
    string ammoType = "None";
    int damage = 0;
    int ammo = 0;
    int maxAmmo = 0;
    int durability = 0;
};
struct Player {
    int hp;
    int maxHP;
    int morale;
    int maxMorale;
    int sus;
    weapons equippedWeapon = { "None","None",0,0,0,0 };
};
struct items {
    string name;
    int amount;
    string effect;
    int effectvalue;
};

struct enemy {
    string enemyname;
    int enemyhp;
    int persuaded;
    int damage;
    string behavior;
};
//player
vector <weapons> weaponInventory;
vector <items> itemInventory;
Player player = { 50, 100, 50,100, 0 };

//enemy
enemy guard{ "Prison Guard", 40,0, 8, "Defensive"};
enemy ssofficer{ "SS Officer",60,20,12, "Tactical"};
enemy heavy{ "Heavy Soldier",80,-10,13,"Aggresive"};
enemy sniper{ "Sniper",20,0,40,"Cautious"};
enemy smuggler{ "Black Market Smuggler", 50, 0, 9, "Tactical" };
enemy twistedssofficer{ "SS Officer?",45,10,20, "Unnatural" };
enemy warden{ "Warden Schwalbe",120,-10,25, "Specialized" };

void checkGameOver() {
    if (player.hp <= 0 || player.morale <= 0) {
        system("cls");
        cout << "You have lost the will to fight. Game over.\n";
        system("pause");
        exit(0);
    }

    else if (player.sus >= 100) {
        system("cls");
        cout << "You have attracted too much attention. Game over.\n";
        exit(0);
    }

    if (player.morale < 0) {
        player.morale = 0;
    }
}

void addWeapon(string name, string ammoType, int damage, int ammo, int maxAmmo, int durability) {
    for (auto& weapon : weaponInventory) {
        if (weapon.name == name) {
            cout << "You already have a " << name << endl;
            return;
        }
    }
    weaponInventory.push_back({ name, ammoType, damage, ammo,maxAmmo, durability });
    cout << "You picked up a " << name << "!\n";
}

void addItem(string name, int amount, string effect, int effectvalue) {
    for (auto& item : itemInventory) {
        if (item.name == name) {
            item.amount += amount;
            cout << "You picked up more " << name << "\nTotal: " << item.amount << endl;
            return;
        }
    }
    itemInventory.push_back({ name,amount,effect,effectvalue });
    cout << "You picked up " << name << "!\n";
}

void addAmmo(string ammoType, int amount) {
    for (auto& item : itemInventory) {
        if (item.name == ammoType) {
            item.amount += amount;
            cout << "You picked up more " << ammoType << "Total: " << item.amount << endl;
            return;
        }
    }
    itemInventory.push_back({ ammoType,amount,"Ammunition"});
    cout << "You found " << amount << " rounds of " << ammoType << "!\n";
}

void reloadWeapon() {
    if (weaponInventory.empty()) {
        cout << "You have no weapons to reload";
    }
    cout << "Current weapons:\n";
    for (size_t i = 0; i < weaponInventory.size(); i++) {
        cout << i + 1 << ". " << weaponInventory[i].name << "(Damage: " << weaponInventory[i].damage << " Ammo: " << weaponInventory[i].ammo << " Durability: " << weaponInventory[i].durability << " (" << weaponInventory[i].ammo << "/" << weaponInventory[i].maxAmmo << " rounds)" << endl;
    }
    cout << "Select a weapon to reload. (0 to cancel): ";
    cin >> choice;
    if (choice == 0) return;
    if (choice < 1 || choice > weaponInventory.size()) {
        cout << "Invalid choice.\n";
        return;
    }
    weapons& selectedWeapon = weaponInventory[choice - 1];

    for (auto& item : itemInventory) {
        if (item.name == selectedWeapon.ammoType && item.amount > 0) {
            int required = selectedWeapon.maxAmmo - selectedWeapon.ammo;
            cout << "You have " << item.amount << " rounds of " << selectedWeapon.ammoType << ".\n";
            int reloadAmount;
            cin >> reloadAmount;

            if (reloadAmount <= 0 || reloadAmount > min(required, item.amount)) {
                cout << "Invalid amount\n";
                return;
            }
            selectedWeapon.ammo += reloadAmount;
            item.amount -= reloadAmount;

            if (player.equippedWeapon.name == selectedWeapon.name) {
                player.equippedWeapon.ammo = selectedWeapon.ammo;
            }

            cout << "Reloaded " << reloadAmount << " rounds into " << selectedWeapon.name <<".\n" << "(Ammo: " << selectedWeapon.ammo << "/" << selectedWeapon.maxAmmo << ")**\n";

            return;
        }
    }
    cout << " You have no ammo for " << selectedWeapon.name << "!\n";
}

void useItem(string name, Player& player) {
    for (auto& item : itemInventory) {
        if (item.name == name) {
            if (item.amount > 0) {
                cout << "You use a: " << name << "\n";
                if (item.effect == "heal") {
                    int oldhp = player.hp;
                    player.hp += item.effectvalue;
                    cout << "You regain " << item.effectvalue << " HP.\n";
                }
                else if (item.effect == "morale") {
                    player.morale += item.effectvalue;
                    cout << "You regain " << item.effectvalue << " Morale.\n";
                    if (player.morale > player.maxMorale) {
                        player.morale = player.maxMorale;
                    }
                }
                else if (item.effect == "sus") {
                    player.sus -= item.effectvalue;
                    cout << "Suspicion has decreased by " << item.effectvalue << "\n";
                }

                item.amount--;
                return;

            }
            else {
                cout << "You're out of " << name << endl;
            }
            
        }
    }
    cout << "Item invalid.\n";
}

void equipWeapon(Player&player,vector <weapons>&weaponInventory) {
    if (weaponInventory.empty()) {
        cout << "No weapons found.\n";
        return;
    }
    else {
        cout << "Available Weapons:\n";
        for (size_t i = 0; i < weaponInventory.size(); i++) {
            cout << i + 1 << ". " << weaponInventory[i].name << " (Damage: "
                << weaponInventory[i].damage << ", Ammo: " << weaponInventory[i].ammo << ")\n";
        }
        cout << "Equipped Weapon: " << player.equippedWeapon.name << endl;
        cout << "Enter the number of the desired weapon to equip, or 0 to exit: ";
        cin >> choice;
        if (choice > 0 && choice <= weaponInventory.size()) {
            player.equippedWeapon = weaponInventory[choice - 1];
            cout << "\nEquipped the " << player.equippedWeapon.name << ".\n";
            system("cls");
        }
        else {
            cout << "Exiting weapon selection.\n";
        }
    }
}


void playerattack(Player& player, enemy& enemy) {
    if (player.equippedWeapon.name.empty() || player.equippedWeapon.name == "None") {
        cout << "No weapons equipped!\n";
        cout << "You decide to use your fists!\n";
        enemy.enemyhp -= 5;
        cout << enemy.enemyname << " takes 5 damage!\n";
    }
    else {
        for (auto& weapon : weaponInventory) {
            if (weapon.name == player.equippedWeapon.name) {
                if (weapon.ammo > 0) {
                    cout << "You fire the " << weapon.name << "!\n";
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    enemy.enemyhp -= weapon.damage;
                    weapon.ammo--;
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    cout << enemy.enemyname << " takes " << weapon.damage << " damage!\n";
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    cout << weapon.name << " Ammo Remaining: " << weapon.ammo << endl;
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    system("cls");
                }
                else if (weapon.durability > 0) {
                    cout << "You attack with " << weapon.name << "!\n";
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    enemy.enemyhp -= weapon.damage;
                    weapon.durability--;
                    player.equippedWeapon.durability--;
                    system("cls");
                    if (weapon.durability == 0) {
                        cout << "Your " << weapon.name << " breaks! You must fight with your fists.\n";
                        this_thread::sleep_for(chrono::milliseconds(1000));
                        player.equippedWeapon = { "None", "None", 0, 0, 0, 0 };
                        system("cls");
                    }
                }
                else {
                    cout << player.equippedWeapon.name << " breaks or no ammo!\n";
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    cout << "You decide to use your fists!\n";
                    enemy.enemyhp -= 5;
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    cout << enemy.enemyname << " takes 5 damage!\n";
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    this_thread::sleep_for(chrono::milliseconds(1000));
                    player.equippedWeapon = { "None","None",0,0,0,0 };
                    system("cls");


                }
            }
        } 
    }
}

void enemyTurn(Player& player, enemy& enemy) {
    int actionchance = rand() % 100;
    if (enemy.behavior == "Aggresive") {
        if (actionchance < 90) {
            cout << enemy.enemyname << " attacks aggresively!\n";
            player.hp -= enemy.damage + 5;
            checkGameOver();
        }
        else {
            cout << enemy.enemyname << " misses!\n";
        }
    }
    else if (enemy.behavior == "Defensive") {
        if (actionchance < 60) {
            cout << enemy.enemyname << " attacks!\n";
            player.hp -= enemy.damage - 2;
            checkGameOver();
        }
        else {
           cout << enemy.enemyname << " raises its guard!\n";
           enemy.enemyhp += 5;
        }
    }
    else if (enemy.behavior == "Tactical") {
        if (actionchance < 50) {
            cout << enemy.enemyname << " demoralizes you!\n";
            player.morale -= enemy.damage * 1.5;
            checkGameOver();
        }
        else {
            cout << enemy.enemyname << " sneak attacks you!\n";
            player.hp -= enemy.damage;
        }
    }
    else if (enemy.behavior == "Cautious") {
        if (actionchance < 30) {
            int ohkochance = rand() % 100;
            if (ohkochance < 10) {
                cout << enemy.enemyname << " delivers a fatal blow to you, killing you instantly...\n";
                player.hp -= player.hp-player.hp;
                checkGameOver();
            }
            cout << enemy.enemyname << " fires a precise shot!\n";
            player.hp -= player.hp/3;
            checkGameOver();
        }
        else {
            cout << enemy.enemyname << " observes you!\n";
        }
    }

    if (enemy.enemyname == "Warden Schwalbe") {
        if (actionchance < 20) {
            cout << "The warden delivers a knockout blow that strikes you in the heart...\n";
            player.morale -= 10;
            player.hp -= 5;
            checkGameOver();
        }
        else if (actionchance < 50) {
            cout << "The warden attacks with a bang...\n";
            player.hp -= enemy.damage;
            checkGameOver();
        }
        else if (actionchance < 70) {
            cout << "The warden gazes you with all his might, as he heals his wounds...\n";
            enemy.enemyhp += 5;
            checkGameOver();
        }
        else {
            cout << "The warden uses his knife, as he fatally targets your weak points...\n";
            player.hp -= enemy.damage * 1.2;
            checkGameOver();

        }
    }

    else if (enemy.enemyname == "SS Officer?") {
        if (actionchance < 20) {
            cout << "The SS officer smiles unnaturally, as his eyes gaze you unnaturally....\n";
            cout << "As he gazes upon you, you feel uncomfortable....\n";
            player.morale -= 10;
            checkGameOver();
        }
        else if (actionchance < 50) {
            cout << "The officer's limbs bend at unnaturally wrong angles, as he strikes you...\n";
            player.hp -= enemy.damage;
            checkGameOver();
        }
        else if (actionchance < 70) {
            cout << "The officer grins at you like a maniac, as if he's from another world...\n";
            checkGameOver();
        }
        else {
           cout << "The officer pierces you with his tendrils, striking you at your critical points...\n";
           cout << "YOU FEEL NOTHING BUT PAIN AND SUFFERING\n";
           player.hp -= enemy.damage*1.2;


        }

     }
}

void persuadeEnemy(enemy& enemy) {
    ifstream charisma;
    charisma.open("charisma.txt");
    charisma >> chr;
    charisma.close();
    int chance = rand() % 100;
    int persuasionIncrease = chr * 2;

    if (enemy.persuaded < 0) {
        cout << "This enemy cannot be persuaded!\n";
    }

    if (chance < 60 + chr * 2) {
        enemy.persuaded += persuasionIncrease;
        system("cls");
        cout << "You attempt to persuade " << enemy.enemyname << endl;
        cout << enemy.enemyname << " hesitates. (Persuasion Meter: " << enemy.persuaded << ")" << endl;
        if (enemy.persuaded >= 100) {
            if (enemy.enemyname == "Prison Guard") {
                cout << "The prison guard leaves you alone.\n";
            }
            else if (enemy.enemyname == "SS Officer") {
                cout << "The SS officer gloats about himself as you sneaked out...\n";
            }
            else if (enemy.enemyname == "Sniper") {
                cout << "The sniper flees...\n";
            }
        }
    }
    else {
        cout << enemy.enemyname << " ignores your words.\n";
    }
}

void inventory() {
    system("cls");
    while (!actionExecuted) {
        cout << "Inventory\n";
        cout << "Equipped Weapon: " << player.equippedWeapon.name << endl;
        cout << "What do you want to check?\n";
        cout << "1. Weapons\n";
        cout << "2. Consumables\n";
        cout << "3. Equip Weapons\n";
        cout << "4. Use items\n";
        cout << "5. Reload weapons\n";
        cout << "6. Exit\n";
        cin >> choice;
        switch (choice) {
        case 1:
            if (weaponInventory.empty()) {
                cout << "No weapons found.\n";
            }
            else {
                cout << "Current weapons:\n";
                for (size_t i = 0; i < weaponInventory.size(); i++) {
                    cout << i + 1 << ". " << weaponInventory[i].name << " (Damage: " << weaponInventory[i].damage << " Ammo: " << weaponInventory[i].ammo << " Durability " << weaponInventory[i].durability <<")" << endl;
                }

            }
            break;
        case 2:
            if (itemInventory.empty()) {
                cout << "No items found.\n";
            }
            else {
                for (auto& item : itemInventory) {
                    cout << "- " << item.name << ": " << item.amount << endl;
                }
            }
            break;

        case 3:
            equipWeapon(player, weaponInventory);
            cout << "Equipped Weapon: " << player.equippedWeapon.name << endl;
            break;

        case 4:
            cout << "You have 3 essential items in your hand. Use them wisely.\n";
            cout << "1. Use First Aid Kit\n";
            cout << "2. Use Rations\n";
            cout << "3. Use Smoke Bombs\n";
            cout << "Choose: ";
            cin >> choice;
            if (choice == 1) {
                useItem("First Aid Kit", player);
            }
            else if (choice == 2) {
                useItem("First Aid Kit", player);
            }
            else if (choice == 3) {
                useItem("Smoke bombs", player);
            }
            else {
                cout << "Invalid choice.\n";
            }
            break;
        case 5:
            reloadWeapon();
            break;
        case 6:
            actionExecuted = true;
            break;

        default:
            cout << "invalid action.\n";
        }


    }
}


void statReset() {
    ifstream knowledge;
    knowledge.open("knowledge.txt");
    knowledge >> knw;
    knowledge.close();

    knw = 0;

    ofstream knowledgeassign;
    knowledgeassign.open("knowledge.txt");
    knowledgeassign << knw;
    knowledgeassign.close();

    ifstream stealth;
    stealth.open("stealth.txt");
    stealth >> ste;
    stealth.close();

    ste = 0;

    ofstream stealthassign;
    stealthassign.open("stealth.txt");
    stealthassign << ste;
    stealthassign.close();

    ifstream charisma;
    charisma.open("charisma.txt");
    charisma >> chr;
    charisma.close();

    chr = 0;

    ofstream charismaassign;
    charismaassign.open("charisma.txt");
    charismaassign << chr;
    charismaassign.close();

    ifstream strength;
    strength.open("strength.txt");
    strength >> str;
    strength.close();

    str = 0;

    ofstream strengthassign;
    strengthassign.open("strength.txt");
    strengthassign << str;
    strengthassign.close();

    ifstream agility;
    agility.open("agility.txt");
    agility >> agi;
    agility.close();

    agi = 0;

    ofstream agilityassign;
    agilityassign.open("agility.txt");
    agilityassign << agi;
    agilityassign.close();

    ifstream perception;
    perception.open("perception.txt");
    perception >> per;
    perception.close();

    per = 0;

    ofstream perceptionassign;
    perceptionassign.open("perception.txt");
    perceptionassign << per;
    perceptionassign.close();
}


//defined in game

void statCheck(const Player& player) {
    ifstream knowledge;
    knowledge.open("knowledge.txt");
    knowledge >> knw;
    knowledge.close();

    ifstream stealth;
    stealth.open("stealth.txt");
    stealth >> ste;
    stealth.close();

    ifstream charisma;
    charisma.open("charisma.txt");
    charisma >> chr;
    charisma.close();

    ifstream strength;
    strength.open("strength.txt");
    strength >> str;
    strength.close();

    ifstream agility;
    agility.open("agility.txt");
    agility >> agi;
    agility.close();

    ifstream perception;
    perception.open("perception.txt");
    perception >> per;
    perception.close();

    cout << "CHARACTER STATS:\n";
    cout << "Name: " << playername << endl;
    cout << "Profession: " << selectedprofession << endl;
    cout << "Knowledge: " << knw << endl;
    cout << "Stealth: " << ste << endl;
    cout << "Charisma: " << chr << endl;
    cout << "Strength: " << str << endl;
    cout << "Agillity: " << agi << endl;
    cout << "Perception: " << per << endl;

    cout << "Current HP: " << player.hp << "/" << player.maxHP << endl;
    cout << "Current Morale: " << player.morale << endl;
    cout << "Current Suspicion: " << player.sus << endl;

    system("pause");
    system("cls");
}



void battle(Player& player, enemy& enemy) {
    system("cls");
    cout << "A " << enemy.enemyname << " appears!\n";

    while (player.hp > 0 && enemy.enemyhp > 0) {
        cout << "HP: " << enemy.enemyhp << endl;
        cout << "1. Attack \n";
        cout << "2. Persuade\n";
        cout << "3. Equip weapons\n";
        cout << "4. Reload Weapon\n";
        cout << "5. Check Stats\n";
        cout << "6. Escape\n";
        cin >> choice;

        switch (choice) {
        case 1:
            playerattack(player, enemy);
            if (enemy.enemyhp <= 0) {
                cout << "You defeated " << enemy.enemyname << "!\n";
                return;
            }
            break;
        case 2:
            persuadeEnemy(enemy);
            if (enemy.persuaded >= 100) {
                return;
            }
            break;
        case 3:
            equipWeapon(player,weaponInventory);
            break;
        case 4:
            reloadWeapon();
            break;
        case 5:
            statCheck(player);
            break;
        case 6:
            if (enemy.enemyname == "SS Officer?" || enemy.enemyname == "Warden Schwalbe") {
                cout << "You cannot escape!\n";
            }
            else {
                cout << "You escaped the battle.\n";
                return;
            }
            break;
        default:
            cout << "Invalid choice.\n";
        }
        if (enemy.enemyhp > 0) enemyTurn(player, enemy);
        checkGameOver();
    }
}

void enemyEncounter() {
    int encounterodds = rand() % 100;
    enemy encounteredEnemy;
    if (encounterodds > 40) { 
        cout << "You hear footsteps... a guard spots you!\n";
        encounteredEnemy = guard;
    }
    else if (encounterodds < 10) { 
        cout << "Someone's pursuing you... an SS officer attacks you!\n";
        encounteredEnemy = ssofficer;
    }
    else if (encounterodds < 20) { 
        cout << "Rumbling sounds are heading towards your way... as a heavy soldier assaults you!\n";
        encounteredEnemy = heavy;
    }
    else if (encounterodds < 30 && (location == "prisonyard" || location == "warehouse")) { 
        cout << "You heard a ringing sound from a distance... as a sniper takes aim at you.\n";
        encounteredEnemy = sniper;
    }
    else {
        return; 
    }
    battle(player, encounteredEnemy);

}

//gameplay loop
void openingScene() {
    cout << "This story is a work of fiction.\n";
    cout << "Names, characters, places and incidents either are products of the author's imagination or are used fictitiously.\n";
    cout << "Any resemblance to actual events or locales or persons, living or dead, is entirely coincidental.\n";
    system("pause");
    system("cls");

    cout << "====================================================================================================\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "1941, World War II, Aftermath of Operation Barbarossa\n\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "???: Report.\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "Dedicated Soldier: Sir! We have made progress in the Soviet Union!\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "Weakened Soldier: And.... we have captured a prisoner.\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "???: And what is this prisoner you speak of?\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "Dedicated Soldier: A Russian, sir. The details are vague, but his intel on Soviet lines has helped us.\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "???: Very good. Describe this Russian to me.\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "======================================================================================================\n";
    system("pause");
    system("cls");
}

void characterCreation() {
    cout << "???: Very well then. Tell me the name of the prisoner.\n";

    while (!characterSelected) {
        cout << "Weakened Soldier: I-I believe it was....\n";
        cin.ignore();
        cin.get(playername, 256);
        cin.ignore(256, '\n');
        cout << "???: Oh? The name is " << playername << ", say? Is that so?\n";
        cout << "Weakened Soldier: Uh-ummmmm...... (Respond with Yes/No)\n";
        cin >> response;
        if (strcmp(response, "Yes") == 0 || strcmp(response, "yes") == 0) {
            cout << "Weakened Soldier: Yes! The prisoner's name is " << playername << "!" << endl;
            characterSelected = true;
        }
        else if (strcmp(response, "No") == 0 || strcmp(response, "no") == 0) {
            cout << "Weakened Soldier: No, it's not that....\n";
        }
    }
    cout << "???: " << playername << " huh, Interesting name....\n";
    while (!professionSelected) {
        cout << "???: Very well. Are they a spy, or a soldier?\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
        cout << "=================\n";
        cout << "|Spy:           |\n";
        cout << "|Starting Stats:|\n";
        cout << "| Knowledge : 6 |\n";
        cout << "| Stealth   : 6 | \n";
        cout << "| Charisma  : 8 | \n";
        cout << "| Strength  : 3 | \n";
        cout << "| Agility   : 4 |\n";
        cout << "| Perception: 4 | \n";
        cout << "=================\n\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
        cout << "=================\n";
        cout << "|Soldier:       |\n";
        cout << "|Starting Stats:|\n";
        cout << "| Knowledge : 4 |\n";
        cout << "| Stealth   : 3 | \n";
        cout << "| Charisma  : 3 | \n";
        cout << "| Strength  : 6 | \n";
        cout << "| Agility   : 8 |\n";
        cout << "| Perception: 7 | \n";
        cout << "=================\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
        cout << "Dedicated Soldier: Sir! They are a... (Respond with Spy/Soldier)\n";
        cin >> profession;
        if (strcmp(profession, "Spy") == 0 || strcmp(profession, "spy") == 0) {
            cout << "Dedicated Soldier: Spy, sir! They are indeed a spy!\n";
            selectedprofession = "spy";
        }
        else if (strcmp(profession, "Soldier") == 0 || strcmp(profession, "soldier") == 0) {
            cout << "Dedicated Soldier: Soldier, sir! They are indeed a soldier!\n";
            selectedprofession = "soldier";
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
        cout << "???: Are you sure about " << playername << " being a " << selectedprofession <<"?\n";
        cout << "Yes/No\n";
        cin >> response;
        if (strcmp(response, "No") == 0 || strcmp(response, "no") == 0) {
            cout << "Dedicated Soldier: Negative sir! It is...\n";
        }
        else if (strcmp(response, "Yes") == 0 || strcmp(response, "yes") == 0) {
            cout << "Dedicated Soldier: Affirmative sir! The information lines up.\n";
            professionSelected = true;
        }
        else {
            cout << "Dedicated Soldier: Negative sir! It is...\n";
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    
    if (selectedprofession == "spy") {
        ifstream knowledge;
        knowledge.open("knowledge.txt");
        knowledge >> knw;
        knowledge.close();

        knw = knw + 6;

        ofstream knowledgeassign;
        knowledgeassign.open("knowledge.txt");
        knowledgeassign << knw;
        knowledgeassign.close();

        ifstream stealth;
        stealth.open("stealth.txt");
        stealth >> ste;
        stealth.close();

        ste = ste + 6;

        ofstream stealthassign;
        stealthassign.open("stealth.txt");
        stealthassign << ste;
        stealthassign.close();

        ifstream charisma;
        charisma.open("charisma.txt");
        charisma >> chr;
        charisma.close();

        chr = chr + 8;

        ofstream charismaassign;
        charismaassign.open("charisma.txt");
        charismaassign << chr;
        charismaassign.close();

        ifstream strength;
        strength.open("strength.txt");
        strength >> str;
        strength.close();

        str = str + 3;

        ofstream strengthassign;
        strengthassign.open("strength.txt");
        strengthassign << str;
        strengthassign.close();

        ifstream agility;
        agility.open("agility.txt");
        agility >> agi;
        agility.close();

        agi = agi + 4;

        ofstream agilityassign;
        agilityassign.open("agility.txt");
        agilityassign << agi;
        agilityassign.close();

        ifstream perception;
        perception.open("perception.txt");
        perception >> per;
        perception.close();

        per = per + 4;

        ofstream perceptionassign;
        perceptionassign.open("perception.txt");
        perceptionassign << per;
        perceptionassign.close();
    }
    else if (selectedprofession == "soldier") {
        ifstream knowledge;
        knowledge.open("knowledge.txt");
        knowledge >> knw;
        knowledge.close();

        knw = knw + 4;

        ofstream knowledgeassign;
        knowledgeassign.open("knowledge.txt");
        knowledgeassign << knw;
        knowledgeassign.close();

        ifstream stealth;
        stealth.open("stealth.txt");
        stealth >> ste;
        stealth.close();

        ste = ste + 3;

        ofstream stealthassign;
        stealthassign.open("stealth.txt");
        stealthassign << ste;
        stealthassign.close();

        ifstream charisma;
        charisma.open("charisma.txt");
        charisma >> chr;
        charisma.close();

        chr = chr + 3;

        ofstream charismaassign;
        charismaassign.open("charisma.txt");
        charismaassign << chr;
        charismaassign.close();

        ifstream strength;
        strength.open("strength.txt");
        strength >> str;
        strength.close();

        str = str + 6;

        ofstream strengthassign;
        strengthassign.open("strength.txt");
        strengthassign << str;
        strengthassign.close();

        ifstream agility;
        agility.open("agility.txt");
        agility >> agi;
        agility.close();

        agi = agi + 8;

        ofstream agilityassign;
        agilityassign.open("agility.txt");
        agilityassign << agi;
        agilityassign.close();

        ifstream perception;
        perception.open("perception.txt");
        perception >> per;
        perception.close();

        per = per + 7;

        ofstream perceptionassign;
        perceptionassign.open("perception.txt");
        perceptionassign << per;
        perceptionassign.close();
    }
   
    cout << "???: Very well then. Transfer " << playername << ", which is that Russian " << selectedprofession << " scumbag to Prison Block A.\n";
    cout << "Dedicated Soldier: Affirmative, Herr Schwalbe!\n";
    cout << "Schwalbe: Let the games... begin.\n";
    system("pause");
    system("cls");
}

void prisonBlock() {
    while (!actionExecuted) {
        cout << "You have awoken, as a prisoner now, who was once a " << profession << " for the Soviet Union. \n";
        cout << "As you observe your surroundings, you see a sleeping guard who is right next to your prison cell, holding the keys. What do you do?\n";
        cout << "1. Persuade the guard (Charisma Check)\n";
        cout << "2. Snatch the guard's keys (Agility Check)\n";
        cout << "3. Check stats\n";
        cin >> choice;
        if (choice == 1) {
            ifstream charisma;
            charisma.open("charisma.txt");
            charisma >> chr;
            charisma.close();
            if (chr >= 5) {
                cout << "You were able to persuade the guard to let you out, on the pretense of visitation.\n";
                actionExecuted = true;
            }
            else {
                cout << "The guard ignores your sayings....\n";
            }
         }
        else if (choice == 2) {
            ifstream agility;
            agility.open("agility.txt");
            agility >> agi;
            agility.close();
            if (agi >= 5) {
                cout << "You were able to snatch the guard's keys while he's asleep, unlocking the cell, and covering your tracks. Impressive!\n";
                actionExecuted = true;
            }
            else {
                cout << "The guard woke up and you retreat back into your cell....\n";
            }
        }
        else if (choice == 3) {
            statCheck(player);

        }
        else {
            cout << "Invalid choice\n";
        }
    }
    cout << "As you broke out of the prison block, you grabbed the sleeping guard's map.\n";
    cout << "NEW ACTION ADDED: MAP VIEWING\n";
    cout << "Keep track of your location with this map!\n";
    system("pause");
    system("cls");
    location = "hallways";
    cout << "Now you're in the hallways, like a rat escaped from the prison. It's now or never.\n";
}

void mapDisplay() {
    system("cls");
    cout << "Map of Freikugeln: \n";
    cout << "========================================================================================\n";
    cout << "|                                |===============| |=============|                     |\n";
    cout << "|                                |Warden's Office| |Security Room|                     |\n";
    cout << "|                                |===============| |=============|                     |\n";
    cout << "|                                       ||              ||                             |\n";
    cout << "|            |======|    |==========|   ||              ||           |============|    |\n";
    cout << "|            | Mess ||||||Connector |||||||||||||||||||||||||||||||||| Warehouse  |    |\n";
    cout << "|            | Hall |    |==========|             ||                 |============|    |\n";
    cout << "|            |======|                             ||                      ||           |\n";
    cout << "|              |||      |============|            ||                      ||           |\n";
    cout << "||========|    |||      |            |            ||                |=============|    |\n";
    cout << "|| Armory |||||||||||||||  Basement  |      |=============|         |   Garage    |    |\n";
    cout << "||========|    |||      |            |      | Prison Yard |         |=============|    |\n";
    cout << "|              |||      |============|      |=============|                            |\n";
    cout << "|              |||                                                                     |\n";
    cout << "|         |============|                                                               |\n";
    cout << "|         |Prison Block|                                                               |\n";
    cout << "|         |============|                                                               |\n";
    cout << "========================================================================================\n";
    cout << "Your current location: " << location << endl;
    if (intelobtained == true) {
        cout << "- BASEMENT WALLS ARE VULNERABLE TO BOMBS. Dealing with that soon.\n";
        cout << "- A new shipment of bombs are in transit.\n";
        cout << "- The garage's vehicles are ready to go... and trucks from there will pick up bombs.\n";
        cout << "- I, Hans Schwalbe, will be giving my grandiose speech on the prison yard!\n";
    }
    system("pause");
    system("cls");

}

void armoryEvent() {
    int armoryevent = (rand() % 4) + 1;
    switch (armoryevent) {
    case 1:
        while (!actionExecuted) {
            cout << "It seems you found a cache of weapons laying around. Could be useful. What will you do?\n";
            cout << "1. Search it (Perception Check)\n";
            cout << "2. Leave it alone.\n";
            cin >> choice;
            if (choice == 1) {
                ifstream perception;
                perception.open("perception.txt");
                perception >> per;
                perception.close();
                if (per > 5) {
                    cout << "After searching the weapons cache, it seems you found some ammo hidden amongst it. Lucky you!\n";
                    addAmmo("9mm Ammo", 10);
                    actionExecuted = true;
                }
                else {
                    cout << "After searching, there's nothing of value there...\n";
                    actionExecuted = true;
                    system("pause");
                    system("cls");
                }
            }
            else if (choice == 2) {
                cout << "You decide to leave it alone. After all, it IS disrespectful to search others' belongings...\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
        }
        break;
    case 2:
        cout << "There's a propaganda poster hanging on the wall next to a locker. You feel uneasy.\n";
        player.morale -= 5;
        system("pause");
        system("cls");
        break;
    case 3:
        while (!actionExecuted) {
            cout << "You opened a locker, and *click*, you accidentally activated a tripwire trap. What will you do?\n";
            cout << "1. Disarm it (Perception and Knowledge Check)\n";
            cout << "2. Make a run for it (Agility Check)\n";
            cin >> choice;
            if (choice == 1) {
                ifstream knowledge;
                knowledge.open("knowledge.txt");
                knowledge >> knw;
                knowledge.close();

                ifstream perception;
                perception.open("perception.txt");
                perception >> per;
                perception.close();

                if (knw > 3 && per > 3) {
                    cout << "You managed to disarm the tripwire... and you feel proud of yourself.\n";
                    player.morale += 6;
                    actionExecuted = true;
                    system("pause");
                    system("cls");
                }
                else {
                    cout << "You fail to disarm the tripwire, and you were injured from the explosion.\n";
                    player.hp -= 10;
                    actionExecuted = true;
                    system("pause");
                    system("cls");
                }
            }
            else if (choice == 2) {
                ifstream agility;
                agility.open("agility.txt");
                agility >> agi;
                agility.close();

                if (agi >= 5) {
                    cout << "You ran away from the tripwire trap as it explodes... You're safe, but the noise definitely attracted some heads.\n";
                    player.sus += 5;
                    actionExecuted = true;
                }
                else {
                    cout << "You fail to run away in time, and you were injured from the explosion.\n";
                    player.hp -= 10;
                    actionExecuted = true;
                }
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 4:
        while (!actionExecuted) {
            cout << "You hear guards' footsteps that are heading towards the armory... what will you do?\n";
            cout << "1. Rig a distraction (Perception Check)\n";
            cout << "2. Fake your own death (Charisma Check)\n";
            cin >> choice;
            if (choice == 1) {
                ifstream perception;
                perception.open("perception.txt");
                perception >> per;
                perception.close();
                if (per > 5) {
                    cout << "You successfully rig the distraction as the guards flee, thinking that a ghost is haunting the armory...\n";
                    if (player.sus > 0) {
                        player.sus -= 5;
                    }
                }
                else {
                    cout << "The guards investigate the armory anyway...\n";
                    player.sus += 10;
                }
                actionExecuted = true;
             
            }
            else if (choice == 2) {
                ifstream charisma;
                charisma.open("charisma.txt");
                charisma >> chr;
                charisma.close();
                if (chr > 5) {
                    cout << "The guards bought your faked death. How charming!\n";
                    if (player.sus > 0) {
                        player.sus -= 5;
                    }
                }
                else {
                  cout << "The guards didn't buy it...\n";
                  player.sus += 10;
                }
                actionExecuted = true;
                
            }
            else {
                cout << "Invalid choice\n";
            }
        }

        break;
    }
    actionExecuted = false;
    
}

void armory() {
    if (weapontaken == true) {
        armoryEvent();
    }
    if (weapontaken == false) {
        cout << "You see an old backpack laying around, and you take it, hoping for the better.\n";
        cout << "NEW ACTION ADDED: Inventory. You can use inventory to store additional things!\n";
        cout << "It seems an notice has been posted next to the weapons cache.\n";
        cout << "It read:\n";
        cout << "===============================================================================\n";
        cout << "NOTICE TO ALL SOLDIERS: \nEVERY WEAPON HAS DIFFERENT CAPABILITIES.\n";
        cout << "Melee weapons does not need ammo, but has a certain amount of durability.\n";
        cout << "Guns, on the other hand, are more powerful, but requires ammo.\n\n";
        cout << "Due to limited inventory, only a submachine gun and a knife is available.\n";
        cout << "NOTE: The submachine gun will have 10 rounds to start.\n";
        cout << "================================================================================\n";
        cout << "What will you pick?\n";
        cout << "1. Submachine gun (+10 ammo)\n";
        cout << "2. Knife\n";
        cin >> choice;
        if (choice == 1) {
            cout << "You obtained the submachine gun alongside its ammo...\n";
            addWeapon("MP40", "9mm Ammo", 25, 30, 15, -1);
            weapontaken = true;
            system("pause");
            system("cls");
        }
        else if (choice == 2) {
            cout << "You obtained the knife...\n";
            addWeapon("Seitengewehr 98", "None", 25, 0, 0, 15);
            weapontaken = true;
            system("pause");
            system("cls");
        }
    }
    armoryEvent();
    cout << "As you enter the armory, multiple things caught your eye. Guns. Uniforms. Many more.\n";
    while (!actionExecuted) {
        cout << "What will you do?\n";
        cout << "1. Rest up\n";
        cout << "2. Find some ammo\n";
        cout << "3. Return to the hallways\n";
        cout << "4. Check stats\n";
        cout << "5. Check map\n";
        if (weapontaken == true) {
            cout << "6. Check inventory\n";
        }
        cin >> choice;

        if (choice == 1) {
            cout << "You rested in the armory... but there is a feeling that the guards will find you...\n";
            player.hp += 10;
            player.morale += 6;
            player.sus +=7;
            checkGameOver();
        }

        else if (choice == 2) {
            int luck = rand() % 100 + 1;
            if (luck >= 1 && luck < 6) {
                cout << "Wow! It seems you got lucky and had a really big find!\n";
                addItem("First Aid Kit", 5, "heal", 20);
                addItem("Ration Packs", 5, "morale", 20);
                addAmmo("9mm Ammo", 20);
                addAmmo("8mm Ammo", 20);
            }
            else if (luck > 7 && luck < 21) {
                cout << "An acceptable find.\n";
                addItem("First Aid Kit", 5, "heal", 20);
                addAmmo("9mm Ammo", 20);
            }
            else if (luck > 20 && luck < 50) {
                cout << "It's only one item. You'll take it...\n";
                addItem("First Aid Kit", 1, "heal", 20);
            }
            else {
                cout << "Sadly, there is nothing of value here...\n";
            }
        }

        else if (choice == 3) {
            cout << "You leave the armory hoping not to attract suspicion.\n";
            location = "hallways";
            actionExecuted = true;
        }
        else if (choice == 4) {
            statCheck(player);
            system("cls");
        }

        else if (choice == 5) {
            mapDisplay();
            system("cls");
        }
        else if (choice == 6) {
            if (weapontaken == true) {
                inventory();
                system("cls");
            }
            else {
                cout << "Invalid choice\n";
                system("cls");
            }
        }
        else {
            cout << "Invalid choice.\n";
            system("cls");
        }
    }
}

void hallways() {
    system("cls");
    while (!actionExecuted) {
        cout << "There are three locations that you can explore now. The mess hall, armory, and basement. What will you do?\n";
        cout << "1. Check out the armory\n";
        cout << "2. Investigate the basement\n";
        cout << "3. Get into the mess hall\n";
        cout << "4. Check stats\n";
        cout << "5. Check map\n";
        if (weapontaken == true) {
            cout << "6. Check inventory\n";
        }
        cin >> choice;

        if (choice == 1) {
            cout << "You check out the armory, prepared to find supplies.\n";
            location = "armory";
            system("pause");
            system("cls");
            actionExecuted = true;
        }
        else if (choice == 2) {
            if (weapontaken == false) {
                cout << "It seems the basement is currently locked...\n";
                system("pause");
                system("cls");
            }
            else {
                cout << "You went into the dark basement....\n";
                location = "basement";
                actionExecuted = true;
            }

        }
        else if (choice == 3) {
            if (weapontaken == false) {
                cout << "You avoid the mess hall in order to not attract suspicion...\n";
                system("pause");
                system("cls");
            }
            else {
                cout << "You went into the mess hall to check out what's going on...\n";
                location = "messhall";
                actionExecuted = true;
            }
        }
       else if (choice == 4) {
           statCheck(player);
         }

       else if (choice == 5) {
            mapDisplay();
          }

      else if (choice == 6) {
           if (weapontaken == true) {
                inventory();
                }
           else {
               cout << "Invalid choice.\n";
                }
            }
      else {
        cout << "invalid choice";
            }
        
    }
}

void basementevent() {
    srand(time(0));
    int basementevent = (rand() % 3) + 1;
    switch (basementevent) {
    case 1:
        while (!actionExecuted) {
            cout << "You hear a groaning sound coming from somewhere... You want to check it, but at the same time, you won't.\n";
            cout << "1. Check it out (Perception and Strength Check)\n";
            cout << "2. Leave it alone.\n";
            cin >> choice;
            if (choice == 1) {
                ifstream perception;
                perception.open("perception.txt");
                perception >> per;
                perception.close();
                ifstream strength;
                strength.open("strength.txt");
                strength >> str;
                strength.close();

                if (per > 4 && str > 5) {
                    cout << "You heard where the groaning comes from... and it turns out, it's a prisoner hauling supplies. You helped him, and he gave something as a reward.\n";
                    addAmmo("8mm Ammo", 9);
                    actionExecuted = true;
                    system("pause");
                    system("cls");
                }
                else {
                    cout << "You're too scared to investigate...\n";
                    player.morale -= 5;
                    actionExecuted = true;
                    system("pause");
                    system("cls");
                }
            }
            else if (choice == 2) {
                cout << "You leave the noise alone.. but it makes you uneasy.";
                player.morale -= 10;
                actionExecuted = true;
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 2:
        cout << "You see skulls in the basement, and they creep you out.\n";
        player.morale -= 10;
        break;
    case 3:
        while (!actionExecuted) {
            cout << "Occult symbols are found in the basement... what will you do?\n";
            cout << "1. Decipher it (Knowledge Check)\n";
            cout << "2. Assume it\n";
            cin >> choice;
            if (choice == 1) {
                ifstream knowledge;
                knowledge.open("knowledge.txt");
                knowledge >> knw;
                knowledge.close();
                if (knw > 6) {
                    cout << "It seems the occult symbols are there for protection....\n";
                    player.morale += 5;
                    system("pause");
                    system("cls");
                }
                else {
                    cout << "You fail to understand the symbols...\n";
                    player.morale -= 20;
                    system("pause");
                    system("cls");
                }
                actionExecuted = true;
            }
            else if (choice == 2) {
                cout << "Something weird is going on here...\n";
                player.morale -= 10;
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    }
    actionExecuted = false;
}

void basement() {
    basementevent();
    while (!actionExecuted) {
        cout << "Ah, the dark and dingy basement.\n";
        cout << "There are only two things notable... a furnace and a cracked wall...\n";
        cout << "1. Check out the furnace\n";
        cout << "2. Investigate the cracked wall\n";
        cout << "3. Return to the hallways\n";
        cout << "3. Check stats\n";
        cout << "4. Check map\n";
        cout << "5. Check inventory\n";
        cin >> choice;
        switch (choice) {
        case 1:
            cout << "You check out the furnace, as it burns brighter than the sun...\n";
            cout << "As you gazed at the furnace, you tripped and fell into the furnace, burning to your death.\n";
            exit(0);
            break;
        case 2:
            if (!bombobtained) {
                cout << "It seems this wall could be blown up by a **bomb**.\n";
            }
            else {
                cout << "You rigged the explosives as the vulnerable basement walls blows up, leaving behind a hole to freedom.\n";
                cout << "CONGRATULATIONS: Bomber.\n";
                actionExecuted = true;
                gameEnded = true;
                return;
            }
            break;
        case 3:
            location = "hallways";
            actionExecuted = true;
            break;
        case 4:
            statCheck(player);
            break;
        case 5:
            mapDisplay();
            break;
        case 6:
            inventory();
            break;
        default:
            cout << "Invalid choice.\n";
        }
    }
}

void messHallEvent() {
    srand(time(0));
    int messhallevent = (rand() % 5) + 1;
    switch (messhallevent) {
    case 1:
        while (!actionExecuted) {
            cout << "A drunk soldier approaches you, thinking he's your comrade... what will you do?\n";
            cout << "1. Persuade him for some intel (Charisma Check)\n";
            cout << "2. Knock him out (Strength Check)\n";
            cout << "3. Leave him alone\n";
            cin >> choice;
            if (choice == 1) {
                ifstream charisma;
                charisma.open("charisma.txt");
                charisma >> chr;
                charisma.close();
                if (chr > 5) {
                    cout << "The soldier's convinced that he's your comrade, giving hints on the prison's vulnerabilities....\n";
                    if (player.sus > 0) {
                        player.sus -= 5;
                    }
                }
                else {
                    cout << "The soldier rats on you....\n";
                    player.sus += 5;
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                ifstream strength;
                strength.open("strength.txt");
                strength >> str;
                strength.close();
                if (str > 5) {
                    cout << "You successfully knocked out the drunken soldier, leaving you with a hint of satisfaction.\n";
                    player.morale += 5;
                }
                else {
                    cout << "The drunken soldier says many slurs degrading you before fainting...\n";
                    player.morale -= 5;
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 3) {
                cout << "You leave the soldier alone to his drunken antics...\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 2:
        while (!actionExecuted) {
            cout << "It seems prisoners are gathered in the mess hall, cleaning up. What will you do?\n";
            cout << "1. Convince one of the prisoners to give you an item (Charisma Check)\n";
            cout << "2. Start a food fight\n";
            cin >> choice;
            if (choice == 1) {
                ifstream charisma;
                charisma.open("charisma.txt");
                charisma >> chr;
                charisma.close();
                if (chr > 5) {
                    cout << "The prisoner gave you some ammo...\n";
                    addAmmo("9mm Ammo", 3);
                }
                else {
                    cout << "The prisoner ignores you...\n";
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                cout << "You started a food fight amongst the prisoners, buying time for you...\n";
                if (player.sus > 0) {
                    player.sus -= 5;
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 3:
        cout << "You see multiple propaganda posters in the mess hall, which makes you uneasy...\n";
        player.morale -= 6;
        system("pause");
        system("cls");
        break;
    case 4:
        cout << "You found one 9mm ammo laying on the floor. Waste not, want not!\n";
        addAmmo("9mm Ammo", 1);
        system("pause");
        system("cls");
        break;
    case 5:
        cout << "A guard spots you suspiciously. Better watch out!\n";
        player.sus += 5;
        system("pause");
        system("cls");
        break;
    }
    actionExecuted = false;
}

void messhall() {
    messHallEvent();
    while (!actionExecuted) {
        cout << "Now you're in the mess hall, the place where guards and prisoners dine.\n";
        cout << "What will you do?\n";
        cout << "1. Eat some food\n";
        cout << "2. Return to the hallways\n";
        cout << "3. Proceed onward\n";
        cout << "4. Check stats\n";
        cout << "5. Check map\n";
        cout << "6. Check inventory\n";
        cin >> choice;
        int servechance;
        int foodchance;
        switch (choice) {
        case 1:
             servechance = rand() % 100 + 1;
             foodchance = rand() % 6 + 1;
            cout << "You treated yourself to the menu, which is ";
            if (servechance <= 30) {
                if (foodchance == 1) {
                    cout << " some superfood soup.\n";
                    ifstream knowledge;
                    knowledge.open("knowledge.txt");
                    knowledge >> knw;
                    knowledge.close();

                    knw = knw + 1;

                    ofstream knowledgeadded;
                    knowledgeadded.open("knowledge.txt");
                    knowledgeadded << knw;
                    knowledgeadded.close();
                }
                else if (foodchance == 2) {
                    cout << " some sneaky schnitzel.\n";
                    ifstream stealth;
                    stealth.open("stealth.txt");
                    stealth >> ste;
                    stealth.close();

                    ste = ste + 1;

                    ofstream stealthadded;
                    stealthadded.open("stealth.txt");
                    stealthadded << ste;
                    stealthadded.close();

                }
                else if (foodchance == 3) {
                    ifstream charisma;
                    charisma.open("charisma.txt");
                    charisma >> chr;
                    charisma.close();

                    chr = chr + 1;

                    ofstream charismaadded;
                    charismaadded.open("charisma.txt");
                    charismaadded << chr;
                    charismaadded.close();
                }
                else if (foodchance == 4) {
                    cout << " leftover canned beef.\n";
                    ifstream strength;
                    strength.open("strength.txt");
                    strength >> str;
                    strength.close();

                    str = str + 1;

                    ofstream strengthadded;
                    strengthadded.open("strength.txt");
                    strengthadded << str;
                    strengthadded.close();

                }
                else if (foodchance == 5) {
                    cout << " some chicken.\n";
                    ifstream agility;
                    agility.open("agility.txt");
                    agility >> agi;
                    agility.close();

                    agi = agi + 1;

                    ofstream agilityadded;
                    agilityadded.open("agility.txt");
                    agilityadded << agi;
                    agilityadded.close();
                }
                else if (foodchance == 6) {
                    ifstream perception;
                    perception.open("perception.txt");
                    perception >> per;
                    perception.close();

                    per = per + 4;

                    ofstream perceptionadded;
                    perceptionadded.open("perception.txt");
                    perceptionadded << per;
                    perceptionadded.close();
                }
            }
            else {
                cout << " nothing...\n";
            }
            break;
        case 2:
            cout << "You return to the hallways...\n";
            location = "hallways";
            actionExecuted = true;
            break;
        case 3:
            cout << "You proceed to the second half of this prison...\n";
            location = "connector";
            actionExecuted = true;
            break;
        case 4:
            statCheck(player);
            break;
        case 5:
            mapDisplay();
            break;
        case 6:
            inventory();
            break;
        default:
            cout << "Invalid choice.\n";
        }
    }
}

void connector() {
    actionExecuted = false;
    while (!actionExecuted) {
        cout << "You are at the place that connects the two halves of this dreaded prison together...\n";
        cout << "What will you do?\n";
        cout << "1. Return to the mess hall\n";
        cout << "2. Go to the warden's office\n";
        cout << "3. Go to the security office\n";
        cout << "4. Go to the warehouse\n";
        cout << "5. Go to the prison yard\n";
        cout << "6. Check stats\n";
        cout << "7. Check map\n";
        cout << "8. Check inventory\n";
        cin >> choice;
        switch (choice) {
        case 1:
            cout << "You return to the mess hall to catch a break...\n";
            location = "messhall";
            actionExecuted = true;
            break;
        case 2:
            cout << "You went into the warden's office, the mastermind behind this hellish nightmare...\n";
            location = "wardensoffice";
            actionExecuted = true;
            break;
        case 3:
            cout << "You proceed to the prison's security room...\n";
            location = "securityroom";
            actionExecuted = true;
            break;
        case 4:
            cout << "You sneaked into the warehouse...\n";
            location = "warehouse";
            actionExecuted = true;
            break;
        case 5:
            cout << "You checked out the prison yard....\n";
            location = "prisonyard";
            actionExecuted = true;
            break;
        case 6:
            statCheck(player);
            break;
        case 7:
            mapDisplay();
            break;
        case 8:
            inventory();
            break;
        default:
            cout << "Invalid choice.\n";
        }
    }
}


void wardensofficeEvents() {
    int wardensofficechance = rand() % 2 + 1;
    int coinflipchance = rand() % 10 + 1;
    switch (wardensofficechance) {
    case 1:
        while (!actionExecuted) {
            cout << "You inspect the warden's desk, however, there are three locked drawers.\n";
            cout << "The first drawer is labeled Links, or left in German.\n";
            cout << "The second drawer is labeled Rechts, or right in German.\n";
            cout << "The third and final drawer is labeled Mitte, or middle in German.\n";
            cout << "What will you do?\n";
            cout << "1. Open Links\n";
            cout << "2. Open Rechts\n";
            cout << "3. Open Mitte\n";
            cin >> choice;
            if (choice == 1) {
                if (coinflipchance > 5) {
                    cout << "This drawer is empty. It seems the warden forgot...\n";
                }
                else {
                    cout << "You open the drawer to find a grenade, and it explodes!\n";
                    player.hp -= 5;
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                if (coinflipchance > 5) {
                    cout << "This drawer is empty. It seems the warden forgot...\n";
                }
                else {
                    cout << "You open the drawer to find some smoke bombs.\n";
                    addItem("Smoke Bomb", 2, "sus", -10);
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 3) {
                if (coinflipchance > 5) {
                    cout << "This drawer is empty. It seems the warden forgot...\n";
                }
                else {
                    cout << "You open the drawer to find raunchy images that tickle you.\n";
                    player.morale -= 5;
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 2:
        while (!actionExecuted) {
            cout << "You see a weird spellbook placed in the warden's desk. Interesting...\n";
            cout << "What will you do?\n";
            cout << "1. Attempt to read it (Knowledge Check)\n";
            cout << "2. Destroy the book.\n";
            cin >> choice;
            if (choice == 1) {
                ifstream knowledge;
                knowledge.open("knowledge.txt");
                knowledge >> knw;
                knowledge.close();

                if (knw > 5) {
                    cout << "After studying the spellbook, your determination ignites.\n";
                    player.morale += 5;
                }
                else {
                    cout << "Forbidden knowledge floods into your mind and strats to wreck your body...\n";
                    player.hp -= 5;
                    player.morale -= 5;
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                cout << "You destroyed the book for good...\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    }
    actionExecuted = false;
}

void wardenOffice() {
    int safecode;
    bool safecracked = false;

    wardensofficeEvents();
    cout << "As you explore the warden's office, you can see everything.\n";
    cout << "Nazi flags, disturbing artwork, and even photographs.\n";
    while (!actionExecuted) {
        cout << "However, there is information there. The problem is, it is locked in a safe. Only the Warden knows the code.\n";
        if (intelobtained == false) {
            cout << "You picked up a note, that reads:\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
            cout << "The Warden keeps a journal, locked away so tight,\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
            cout << "But numbers whisper in candlelight.\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
            cout << "Not on the page, nor in the air,\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
            cout << "But under the wax, if you dare.\n";
            this_thread::sleep_for(chrono::milliseconds(1000));
        }
        else{
            cout << "You have now obtained the vital intel...\n";
        }
        cout << "What will you do?\n";
        cout << "1. Search the desk\n";
        cout << "2. Inspect the paintings\n";
        cout << "3. Look at his collection\n";
        cout << "4. Search the bookshelves\n";
        cout << "5. Enter the code\n";
        cout << "6. Check notes\n";
        cout << "7. Return to connector\n";
        cout << "8. Check stats\n";
        cout << "9. Check inventory\n";
        cout << "10. Check map\n";
        cin >> choice;

        switch (choice) {
        case 1:
            if (searchedDesk == false) {
                cout << "You searched the desk, as the desk has a burning candle on it, just like the note said.\n";
                cout << "As such, you melted the candlewax on the warden's desk, which reveals a number. 7.\n";
                system("pause");
                system("cls");
            }
            else {
                cout << "You have already searched this place.\n";
            }
            break;
        case 2:
            if (searchedPainting == false) {
                cout << "As such, you searched the paintings that are neatly arranged.\n";
                cout << "Four paintings. And a note placed under them.\n It read:\n";
                cout << "Four figures stand tall, their eyes carved deep,\n";
                cout << "One looks west, where secrets keep.\n";
                cout << "Count their gazes, left to right,\n";
                cout << "Their silent stares will grant you sight.\n";
                system("pause");
                system("cls");
                searchedPainting = true;
            }
            else {
                system("cls");
                cout << "After reading the second note, you are intrigued by the paintings.\n";
                cout << "A painting of Adolf Hitler, raising his arm to the left side.\n";
                cout << "Another painting of a desert, with an eagle soaring to catch the mouse on the left.\n";
                cout << "A painting of Otto Von Bismarck, facing to the right\n.";
                cout << "Finally, a painting of himself, facing to the west. That narcissist.\n";
                system("pause");
                system("cls");
            }
            break;
        case 3:
            if (searchedCollection == false) {
                cout << "As such, you inspected the warden's collection of trinkets. So beautiful. And then, you found a note.\n";
                cout << "A hero once stood, now buried in stone,\n";
                cout << "His name engraved, yet rarely known.\n";
                cout << "Three digits follow, carved with pride,\n";
                cout << "Find his number, and turn the tide.\n";
                system("pause");
                system("cls");
                searchedCollection = true;
            }
            else {
                cout << "After reading the third note, you pay attention to a golden plaque.\n";
                cout << "A war hero the warden adored, codenamed Ixion.\n";
                cout << "But... why does the first two letters in Ixion colored red?\n";
                system("pause");
                system("cls");
            }
            break;
        case 4:
            if (searchedBookshelves == false) {
                cout << "You saw the warden's book collection, a man of culture he is.\n";
                cout << "But, a note caughts your eye. It read:\n";
                cout << "One key to rule, yet not of brass,\n";
                cout << "It isn’t seen, yet you must pass.\n";
                cout << "Where ink meets blood, a number lies,\n";
                cout << "A page torn out, but still it cries.\n";
                system("pause");
                system("cls");
                searchedBookshelves = true;
            }
            else {
                while (!satisfied) {
                    cout << "There are several books there. Which one do you want to read?\n";
                    cout << "1. Order and Obedience\n";
                    cout << "2. Mein Kampf\n";
                    cout << "3. Naturea Historica\n";
                    cout << "4. Leave\n";
                    cin >> choice;
                    if (choice == 1) {
                        cout << "It seems this book is about keeping people in order. However... it seems that the second page is missing...\n";
                    }
                    else if (choice == 2) {
                        cout << "Adolf Hitler's book. How intriguing.\n";
                    }
                    else if (choice == 3) {
                        cout << "This book explains about natures and histories.\n";
                    }
                    else if (choice == 4) {
                        cout << "You move on from the bookshelves...\n";
                        satisfied = true;
                    }
                    else {
                        cout << "Invalid choice.\n";
                    }

                }
            }
            break;
        case 5:
            while (!safecracked) {
                cout << "An ornate safe is placed to the right of the warden's desk...\n";
                cout << "Now, you enter the combination which is... (0 to cancel)";
                cin >> safecode;
                if (safecode == 7392) {
                    cout << "The safe is unlocked without hesitation, as you see what's inside.\n";
                    cout << "Vulnerable escape routes, manifests, and even... where the warden would go.\n";
                    cout << "You quickly jot this intel down in your map.\n";
                    intelobtained = true;
                    safecracked = true;
                }
                else if (safecode == 0) {
                    cout << "You leave the safe....\n";
                    safecracked = true;
                }
                else {
                    cout << "It doesn't seem to budge.\n";
                }
            }
            break;
        case 6:
            if (searchedDesk == true) {
                cout << "The Warden keeps a journal, locked away so tight,\n";
                cout << "But numbers whisper in candlelight.\n";
                cout << "Not on the page, nor in the air,\n";
                cout << "But under the wax, if you dare.\n";
            }

            if (searchedPainting == true) {
                cout << "Four figures stand tall, their eyes carved deep,\n";
                cout << "One looks west, where secrets keep.\n";
                cout << "Count their gazes, left to right,\n";
                cout << "Their silent stares will grant you sight.\n";
            }

            if (searchedCollection == true) {
                cout << "A hero once stood, now buried in stone,\n";
                cout << "His name engraved, yet rarely known.\n";
                cout << "Three digits follow, carved with pride,\n";
                cout << "Find his number, and turn the tide.\n";
            }

            if (searchedBookshelves == true) {
                cout << "One key to rule, yet not of brass,\n";
                cout << "It isn’t seen, yet you must pass.\n";
                cout << "Where ink meets blood, a number lies,\n";
                cout << "A page torn out, but still it cries.\n";
            }

            system("pause");
            system("cls");

            break;
        case 7:
            location = "connector";
            actionExecuted = true;
            break;
        case 8:
            statCheck(player);
            break;
        case 9:
            mapDisplay();
            break;
        case 10:
            inventory();
            break;
        default:
            cout << "Invalid choice.\n";
       }
    }
}
    
void securityroomEvents() {
    int securityeventchance = rand() % 3 + 1;
    switch (securityeventchance) {
    case 1:
        while (!actionExecuted) {
            cout << "A radio is playing in static.... and it seems like it wants you to tune the frequency.\n";
            cout << "What will you do?\n";
            cout << "1. Tune the frequency\n";
            cout << "2. Leave it alone\n";
            cin >> choice;
            if (choice == 1) {
                cout << "As you tune the frequency in the radio... it feels otherworldly...\n";
                player.morale -= 5;
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                cout << "You leave it alone....\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.";
            }
        }
        break;
    case 2:
        while (!actionExecuted) {
            cout << "It seems there's a weapons cache. There's a bunch of weapons there.\n";
            cout << "What will you do?\n";
            cout << "1. Take the Mauser K98\n";
            cout << "2. Take the Luger pistol\n";
            cin >> choice;
            if (choice == 1) {
                cout << "You take the Mauser sniper rifle...\n";
                addWeapon("Mauser K98", "8mm Ammo", 40, 3, 6, -1);
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                cout << "You take the Luger pistol....\n";
                addWeapon("Luger", "9mm Ammo", 20, 9, 20, -1);
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.";
            }
        }
        break;
    case 3:
        cout << "You see multiple images of inmates being brutally tortured. The images scare you.\n";
        player.morale -= 6;
        break;
    }
    actionExecuted = false;
}

void securityRoom() {
    securityroomEvents();
    int reducedsus = rand() % 10;
    cout << "Now, you are at the heart of it all. The security room. Filled with surveillance equipment.\n";
    cout << "The atmosphere feels oppressive, as there are radio equipment alongside soldiers that are too busy to notice you.\n";
    while (!actionExecuted) {
        cout << "What will you do?\n";
        cout << "1. Tamper with the equipment\n";
        cout << "2. Return to the connector\n";
        cout << "3. Check stats\n";
        cout << "4. Check map\n";
        cout << "5. Check inventory\n";
        cin >> choice;
        switch (choice) {
        case 1:
            cout << "It seems by tampering the equipment, it lowers your suspicion...\n";
            if (player.sus > 0) {
                player.sus = player.sus - reducedsus;
            }
            break;
        case 2:
            location = "connector";
            actionExecuted = true;
            break;
        case 3:
            statCheck(player);
            break;
        case 4:
            mapDisplay();
            break;
        case 5:
            inventory();
            break;
        default:
            cout << "Invalid choice.\n";
        }
    }
}

void warehouseEvent() {
    int warehouseevent = rand() % 3 + 1;
    int randomitem = rand() % 4 + 1;
    switch (warehouseevent) {
    case 1:
        while (!actionExecuted) {
            cout << "You spotted a cargo box that is peculiar.\n";
            cout << "It says: 1941. Sender: " << playername << ". What could this be?\n";
            cout << "What will you do?\n";
            cout << "1. Open the cargo box\n";
            cout << "2. Throw it away\n";
            cin >> choice;
            if (choice == 1) {
                cout << "As you open the box, you realize it's you... lifeless in a fetal position.\n";
                cout << "When you try to look back, the figure snatches you... and beheads you.\n";
                exit(0);
            }
            else if (choice == 2) {
                cout << "You leave it alone to avoid further consequences.\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 2:
        cout << "You find a delivery of goods from a truck recently delivered and you intercepted it...\n";
        if (randomitem == 1) {
            cout << "You got lucky.... you intercepted a shipment of weapons.\n";
            addAmmo("9mm Ammo", 9);
            addAmmo("8mm Ammo", 9);
            addWeapon("MG 13", "8mm Ammo", rand()%100 + 20, 6, 9, -1);
            system("pause");
            system("cls");
        }
        else if (randomitem == 2 || randomitem == 3) {
            cout << "You gained some essential supplies.\n";
            addItem("First Aid Kit", 3, "Heal", 20);
            system("pause");
            system("cls");
        }
        else {
            cout << "The shipment you intercepted was just fish for the mess hall.\n";
            system("pause");
            system("cls");
        }
        break;
    case 3:
        cout << "You see smugglers talking to an SS officer regarding weapons... but something is weird with that officer. He's motionless.\n";
        cout << "What will you do?\n";
        cout << "1. Approach the SS officer\n";
        cout << "2. Observe them\n";
        cout << "3. Walk away\n";
        cin >> choice;
        if (choice == 1) {
            cout << "As you approach the SS officer, the smugglers attack you first for disrupting business...\n";
            battle(player, smuggler);
            system("pause");
            system("cls");
            cout << "After the battle, the SS officer grins at you.\n";
            cout << "It stares.\n At you.\n UnNATuRALLY.\n";
            battle(player, twistedssofficer);
            system("pause");
            system("cls");
        }
        else if (choice == 2) {
            cout << "You observe them, but the sight of the uncanny officer creeps you out...\n";
            player.morale -= 10;
            system("pause");
            system("cls");
        }
        else if (choice == 3) {
            cout << "You decide to leave befor it's too late.\n";
            system("pause");
            system("cls");
        }
        else {
            cout << "Invalid choice.\n";
        }
        break;

    }
    actionExecuted = false;
}

void Warehouse() {
    warehouseEvent();
    while (!actionExecuted) {
        cout << "The warehouse. Where things come and go, and a chance to freedom.\n";
        cout << "There are several things you can do now... now that it is cargo season.\n";
        cout << "What will you do?\n";
        cout << "1. Find an open cargo box\n";
        cout << "2. Find useful supplies\n";
        cout << "3. Go to the garage\n";
        cout << "4. Return to the connector\n";
        cout << "5. Check map\n";
        cout << "6. Check stats\n";
        cout << "7. Check inventory\n";
        cin >> choice;
        switch (choice) {
        case 1:
            cout << "You find two open cargo boxes. One is filled with smelly fish, and the other, a weapons cache. What will you pick?\n";
            cout << "1. Weapons cache\n";
            cout << "2. Smelly fish\n";
            cin >> choice;
            if (choice == 1) {
                cout << "You hid in a weapons cache, hoping to not be found.\n";
                cout << "As the guard opens the box you were hiding in, you were caught!\n";
                cout << "You were arrested and was sentenced to death.\n";
                actionExecuted = true;
                gameEnded = true;
                return;
            }
            else if (choice == 2) {
                cout << "You hid in the box of smelly fish, hoping the guards wouldn't notice.\n";
                cout << "Fortunately, they didn't notice, as they cannot stand the stench of the fish.\n";
                cout << "The cargo box was loaded into a truck, and you leave undetected...\n";
                cout << "CONGRATULATIONS: You escaped by sneaking into a box of smelly fish.\n";
                actionExecuted = true;
                gameEnded = true;
                return;
            }
                break;
        case 2:
            if (intelobtained == true) {
                cout << "It seems that the intel is true. You found some bombs. Time to head into the basement!\n";
                bombobtained = true;
            }
            break;
        case 3:
            location = "garage";
            actionExecuted = true;
            break;
        case 4:
            location = "connector";
            actionExecuted = true;
            break;
        case 5:
            mapDisplay();
            break;
        case 6:
            statCheck(player);
            break;
        case 7:
            inventory();
            break;
        default:
            cout << "Invalid choice.\n";
        }
    }
}

void GarageEvents() {
    int garageevent = rand() % 2 + 1;
    switch (garageevent) {
    case 1:
        while (!actionExecuted) {
            cout << "A gruff mechanic calls you out...\n";
            cout << "Mechanic: Hey you, yes you! Gimme that red funny lookin' wrench right outta my toolbox, ya hear?\n";
            cout << "You don't know. But there are three wrenches. Pick one, he said.\n";
            cout << "1. The red, funny looking one,\n";
            cout << "2. The yellow, star-shaped one,\n";
            cout << "3. The black, big one.\n";
            cin >> choice;
            if (choice == 1) {
                cout << "Thanks, kid! I owe ya one!, the mechanic said as he happily fixed a broken truck.\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2 || choice == 3) {
                cout << "This isn't the wrenches I wanted!, the mechanic shouted as you, as guards begin to notice the commotion...\n";
                player.sus += 10;
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 2:
        cout << "You hear the roaring sounds of the engines as you pass by.\n";
        system("pause");
        system("cls");
        break;
    }
    actionExecuted = false;
}

void garage() {
    GarageEvents();
    while (!actionExecuted) {
        cout << "The garage. Filled with vehicles and many more. You couldn't resist grabbing one tank or truck to blow yourself to freedom.\n";
        cout << "It seems that you could cause chaos once you can commandeer a tank... Or pose as a truck driver and drive off...\n";
        cout << "What will you do?\n";
        cout << "1. Commandeer a tank (Agility and Perception Check)\n";
        cout << "2. Masquerade as a truck driver (Charisma and Stealth Check)\n";
        cout << "3. Return to the warehouse\n";
        cout << "4. Check map\n";
        cout << "5. Check stats\n";
        cout << "6. Check inventory\n";
        cin >> choice;
        if (choice == 1) {
            ifstream agility;
            agility.open("agility.txt");
            agility >> agi;
            agility.close();
            ifstream perception;
            perception.open("perception.txt");
            perception >> per;
            perception.close();
            if (agi > 5 && per > 5) {
                cout << "As you board the tank, you commandeered it like the beast it was.\n";
                cout << "The tank fired all its glory on the enemy, blasting them into bits while causing chaos.\n";
                cout << "You escaped.... with the fury of a tank on your side.\n";
                cout << "CONGRATULATIONS: You escaped with a tank.\n";
                actionExecuted = true;
                gameEnded = true;
                return;
            }
            else {
                cout << "You can't handle the tank as you can't figure out the controls, and you tried to find another vehicle.\n";
            }
        }
        else if (choice == 2) {
            ifstream charisma;
            charisma.open("charisma.txt");
            charisma >> chr;
            charisma.close();

            ifstream stealth;
            stealth.open("stealth.txt");
            stealth >> ste;
            stealth.close();
            if (chr > 5 && ste > 6) {
                cout << "You convinced the guards to let you drive a truck for delivery.\n";
                cout << "Little do they know, they're about to be tricked.\n";
                cout << "You drove off your truck to freedom...\n";
                cout << "CONGRATULATIONS: You escaped with a truck.\n";
                actionExecuted = true;
                gameEnded = true;
                return;
            }
            else {
                cout << "The guards did not buy your story...\n";
            }
        }
        else if (choice == 3) {
            location = "warehouse";
            actionExecuted = true;
        }
        else if (choice == 4) {
            mapDisplay();
        }
        else if (choice == 5) {
            statCheck(player);
        }
        else if (choice == 6) {
            inventory();
        }
        else {
            cout << "Invalid option.";
        }
    }
}

void yardEvent() {
    int yardevent = rand() % 3 + 1;
    switch (yardevent) {
    case 1:
        while (!actionExecuted) {
            cout << "A prisoner mumbles to himself in backwards...\n";
            cout << "He seems to stare at you..\n";
            cout << "What do you do?\n";
            cout << "1. Approach him,\n";
            cout << "2. Run away.\n";
            cin >> choice;
            if (choice == 1) {
                cout << "You don't understand what he's talking about...\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                cout << "You flee from the scene....\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    case 2:
        while (!actionExecuted) {
            cout << "You see two prisoners fighting against each other.\n";
            cout << "It is their fight, but you don't want to see someone getting hurt.\n";
            cout << "What do you do?\n";
            cout << "1. Approach them. (Charisma Check)\n";
            cout << "2. Run away.\n";
            cin >> choice;
            if (choice == 1) {
                ifstream charisma;
                charisma.open("charisma.txt");
                charisma >> chr;
                charisma.close();
                if (chr > 5) {
                    cout << "You calmed both inmates down...\n";
                    cout << "That good deed gave you a little bit of morale.\n";
                    player.morale += 5;
                }
                else {
                    cout << "Both inmates keep fighting...\n";
                }
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else if (choice == 2) {
                cout << "You flee from the scene....\n";
                actionExecuted = true;
                system("pause");
                system("cls");
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
        break;
    }
}

void prisonYard() {
    while (!actionExecuted) {
        cout << "The prison yard. The open fields, where inmates decide to interact.\n";
        cout << "This is where you can obtain improvised weapons.\n";
        if (intelobtained == true) {
            cout << "It seems the warden is preparing a speech...\n";
        }
        else {
            cout << "Some soldiers are preparing for something...";
        }
        cout << "What will you do?\n";
        cout << "1. Talk to the slick inmate\n";
        cout << "2. Talk to the weird inmate\n";
        if (!intelobtained) {
            cout << "3. Observe the soldiers\n";
        }
        else {
            cout << "3. Hear the warden's speech\n";
        }
        cout << "4. Return to the connector\n";
        cout << "5. Check map\n";
        cout << "6. Check stats\n";
        cout << "7. Check inventory\n";
        cin >> choice;
        if (choice == 1) {
            ifstream agility;
            agility.open("agility.txt");
            agility >> agi;
            agility.close();
            if (agi > 5) {
                cout << "You played some poker with the slick inmate.\n";
                cout << "The inmate lost, and he gave you an item.\n";
                addItem("Ration Packs", 1, "morale", 20);
            }
            else {
                cout << "You lost....\n";
            }
        }
        else if (choice == 2) {
            ifstream charisma;
            charisma.open("charisma.txt");
            charisma >> chr;
            charisma.close();
            if (chr > 5) {
                cout << "You talked to the weird inmate, who seems connected with you.\n";
                cout << "For your troubles, he gave you a wrench.\n";
                addWeapon("Wrench","None",30,0,0,10);
            }
            else {
                cout << "You don't understand the weird inmate...\n";
            }
        }
        else if (choice == 3) {
            if (!intelobtained) {
                cout << "You see some soldiers preparing something, but you don't know about it.\n";
            }
            else {
                cout << "The warden steps up on the stage, as he delivers a speech.\n";
                cout << "You disrupted the speech by firing a gun, as the warden is shocked.\n";
                cout << "Chaos ensues, as the inmates riot to their freedom. Including you.\n";
                cout << "You prepared to battle the warden, the mastermind behind your suffering.\n";
                system("pause");
                battle(player, warden);

                cout << "At last, Warden Hans Schwalbe has fallen.\n";
                cout << "You've did it. You've liberated Freikugeln as the inmates tear this place apart upside down.\n";
                cout << "CONGRATULATIONS: Freikugeln Liberator.";
                actionExecuted = true;
                gameEnded = true;
                return;
            }
        }
        else if (choice == 4) {
            location = "connector";
            actionExecuted = true;
        }
        else if (choice == 5) {
            mapDisplay();
        }
        else if (choice == 6) {
            statCheck(player);
        }
        else if (choice == 7) {
            inventory();
        }
        else {
            cout << "Invalid option.";
        }
    }
}


void gameplayLoop() {
    system("cls");
    openingScene();
    characterCreation();
    prisonBlock();
    while (!gameEnded) {
        if (location == "hallways") {
            system("cls");
            actionExecuted = false;
            hallways();
            checkGameOver();
        }
        else if (location == "armory") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            armory();
        }
        else if (location == "basement") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            basement();
            checkGameOver();
        }
        else if (location == "messhall") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            messhall();
            checkGameOver();
        }
        else if (location == "connector") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            connector();
        }
        else if (location == "warehouse") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            Warehouse();
            checkGameOver();
        }
        else if (location == "garage") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            garage();
            checkGameOver();
        }
        else if (location == "wardensoffice") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            wardenOffice();
            checkGameOver();
        }
        else if (location == "securityroom") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            securityRoom();
            checkGameOver();
        }
        else if (location == "prisonyard") {
            system("cls");
            actionExecuted = false;
            enemyEncounter();
            prisonYard();
            checkGameOver();
        }
    }
}

void colorChoice() {
    int color;
    cout << "Pick a color and text color!\n";
    cout << "Choices are:\n 1. Red and Green\n 2. Blue and Red\n 3. Yellow and Purple\n 4. Default\n";
    cout << "Pick one: ";
    cin >> color;
    if (color == 1) {
        system("Color 4A");
    }
    else if (color == 2) {
        system("Color 1C");
    }
    else if (color == 3) {
        system("Color 2D");
    }
    else if (color == 4) {
        system("Color 0F");
    }
    else {
        cout << "Invalid choice";
        system("cls");
    }
}
int main()
{
    srand(time(0));
    statReset();
    cout << "===============================================================================\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "######## ########  ######## #### ##    ## ##     ##  ######   ######## ##       \n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "##       ##     ## ##        ##  ##   ##  ##     ## ##    ##  ##       ##       \n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "##       ##     ## ##        ##  ##  ##   ##     ## ##        ##       ##       \n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "######   ########  ######    ##  #####    ##     ## ##   #### ######   ##   \n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "##       ##   ##   ##        ##  ##  ##   ##     ## ##    ##  ##       ##      \n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "##       ##    ##  ##        ##  ##   ##  ##     ## ##    ##  ##       ##      \n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "##       ##     ## ######## #### ##    ##  #######   ######   ######## ######## \n";
    this_thread::sleep_for(chrono::milliseconds(1000));
    cout << "===============================================================================\n";
    this_thread::sleep_for(chrono::milliseconds(3000));
    cout << "Select an option: \n";
    cout << "1. Start Game\n";
    cout << "2. Quit\n";
    cout << "3. Color Change\n";
    cin >> titlescreen;
    switch (titlescreen) {
    case 1:
        gameplayLoop();
        break;
    case 2:
        return 0;
        break;
    case 3:
        colorChoice();
        system("cls");
        main();
        break;
    case 188518:
        system("cls");
        cout << "@@@@@@@@@@@@@@@@@@@@%@@@@@@@@@%##@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@@@@@@@@@@@@@@@@@@@@@%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "%%@@@@@@@@@@@@@@@@@@%@@@@@@@@@@@@@@@@@@@@%%%%%@@@@@@@@@@@@@@@@@@@@@@%@@@@@@@%@@@@@@@@@@@%#@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@@%%%%%%@@@@@@@@@@@@@@@%%%%%@@@@@%@@@@@@@@@@@@@@@%%@@@@@@@@@@@@@@@@\n";
        cout << "@%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@@@@@@%%#########%%%%%@@@@@%%@@%@%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%@@@%%%####**#####****#########%@@@%%%%%%%%@@%%@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "%%%@@@@@@@@@@@@@@@@@@@@@@@@%%%%@@@%#*#***++++##****+**********####%@%%%%%%%%%%%@%*%@@@@@@@@@@@@@@@@@@@\n";
        cout << "%%%%@@@@@@@@@@@@@@@@@@#%@%%%%@@%##%***++++++*#**++++*++++*********##%%%%%%%%%%%%@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@%@@@@@@@@@@@@%@@@@@@@@%%%@@%##*++**********+++++++=====+++++++*#**###%@%%%%%%%%@@@@@%%@@@@@@@@@@@@@@\n";
        cout << "@@@%%@@@@@@@@@@@@@@@@@@%%@@%##**+++++++****+++*******++==+====+*+*****##%@%%%%%%%%@@@@%%%%@@@@@@@@@@@@\n";
        cout << "@@%%@@@@@@@@@@@@%%@%%@%%@@###**++++++++++****########**+++++==+**+++++*##%@@%%%%%%%%%%%#%@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@%%%@%%%%%@@#*****+++****+++**##%%%%%%%%%%#+++*++++%*#####*++#@@%%%%%%%%%%%%%@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@%%%%%@@#**+++*+++++===+###%@%%%%%%%%%%%*+***+++#%%@@%%%#*+#@%%%%%%%%%%@%%%@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@%@@%%@@#*+++++++++++++*#%%%%@@@@@@@@%%%%#*##**+++@%@%@@%%#***@@%#%%%%%%%%%%@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@%%%#%%%@@%*******+++***+*##%%%@@@@@@@@@@%%%*+*%#*+++#@@@@@@%%*+*%@@%#%%%%%%%%%@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@%%%%%%%@%##****%%%#*+*#**#@%@%@@@@@@@@@@@%#*+*###**++%@@@@@@%%***@@%%#%%%%%%%%@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@%%%%%%%@@####**%%%@%%##***#%%%%@@@@@@@@@@@%***##%#**++*@@@@@%%*#**#@@%##%%@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@%%%%%%%@@%##**#%@@@@%#**#%@@%%%@@@@@@@@@@%**###%#*###*#%@%@%%%##%#%@@%##%%@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@%%%%%@@%###*##%@@@@@@@@@@@@%%%%@@@@@@@@%*##%%%@%%##%#%##%@%%%@%@%%@%##%%%%%%@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@%%%%@@%****##%@@@@@#***+*%%@@@@@@@%##*##%%@@%@%%%%%%%###*+++*@@#%@%##%%*%%%%@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@%%%%%@%***+**#%%%@#*+=++++******#####%%%@%%#####*#####%%##*++**#%%%###%%%%%%@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@%%%%%%%@%++++++*####*+===+++**#####%%%@%%#######%#*##*#**%%%%#**+*%@%###%%%@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@%%@@@@%%%%%%%@%#*++==++**#++++*****##%%%%%%%%###*+*#*****##***+*#%%#%#*#%@%##*%%%@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@%%%@@@##**##%####*#####%%%@@%%#**###**#*+***+**+##*+*+++##%@%##%@%#%%%%%%@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@%%@@#*##%###%%%@@@@@@@@#******#########%%@@@@%@@###+*++##%%#%@@%#%%%%%%@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@%%%%%@%%#######%#**********##**##*#%@@@@@@@@@@@@@@@@@@@@###%@@@@%%#%%%@@%@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@%%%%%%@@*#*#*#*#%#%%#*#**#*+#***#@@@@@@@@@@@@@@@@@@@@@@%%%%@@@%@%%%%%%%%%%@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@%%%%%%@%**###**%%*#%@@%%@%%@@@@@@@@@@@@@@@@@@@@@@@@%%@@@%%%%@%%%%%%%%%%%%@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@%%%%%%%%@%@@%#*+**#%@%###%%%##%@@@@@@@@@@@@@@@@@@@@@@@%%#@@%%#%@%%%#%%%%%%%@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@%%%%%%%%%@@%%%%@%#%*+***##%##%%%%%##%@@@@@@@@@@@@@@@@@@@@%#%##@@%@@#%%%%%%%%%@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@%%%%%%%%%%%@%%%%@%##*+*#**#%%#*#%%%%*#%#@@@@@@@@@@@@@@@@%%##%%%%@@#%%%%%%%%@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@%%%%@@@%@@@@%%%%@@@##***###%###*######**@*#%@@@@@@@@@@@%%%%%@#*#@%#%#%%%%@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@%@@@@@%%%%@@@#*###**#######***#####*##+*#%%@%%#%%%%%#*##@@%%%#%%%%@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%%%%@@@%##%%#####*##*###**#%%%%%**#++*+***#*##@@@##%%%%%@%%@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%%%%%@@@@@@%######*****#########%%@%@%%%%%@@%#@@%#%%%%%%@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%#%%%%%%@@@@@@@%%%######**%%###%@%######*#%@@@%%%%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%%%%%%@@@@@@@@%%%%%#####%%#*####%@@@@@%%%%%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%%%%%%%%%%%%%@@@@@@@@@@@@@@@@@@@%%%%%%%%%%%%%@@@@@%@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%%#%%%%%#%%%%%###%%%%%%%%%%%%%%%%%%%%%%%%%%%%@@@%%%@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%%%%%%%%%%%%#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@@@%%%@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%%%%#%%%%%%%%%%%%%%%%%@@@@%%%%%%%%%%%%%%%%%%%@%%%%@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@%##@@@@@@@@@@%%%%%%%%%%%%########%##%%%%%%%%@@@@@%#%%%%%%%%%%%%%%%%%@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@%##%@@@@@@@@%%%%%%%%%%%%%#############%%%%%@@@@@%%%%%%%%%%%%@%%%%%%%%@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@%%%%%##%@%%@@@@@@%%%#%%%%%%%%%#%%%%#####%%%%%%@@@@@%####%%%%%%%%%%%%%@%%%@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@%@%%%%##%@@@@@@@@@@%%%%##%%%%%%%%%%%####%@@@@@@@@@@%#%###%%%%%%%%%%%%%%%@%@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@%@@@@@@@@@@%%%####%%%%%%%%%%%%%#%@@@@@@@@@@@%#####%%%%%%%%%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@@%%##%##%%%%%%%#%%%%%%%%%%%%@@@@@@@@@@%%%%%%%%%%%@%@@@@@@@@@@@@@@@%@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%%%%####%%%%%%%%#%%%%%%%%##%@@@@@@@@@@@@%%%%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@%%@@@@@%%%@@@@@@@@%%%#%#%%%%%%%%%%%%%%%%%%##%@@@@%%%@@@@##%%%%@@@@@@%%@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@%####%@@@@@@@%%%%%%%%#%#%%%%%%%%%%%%%%@@@@@%#%##%###%%@%@%@@@@%##@@%@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@%%%####@@@@@@%%%%%%%%%%#%%%%%%%%%%#%%%@@@@%%%#%%%%%%%@@@@@@@@@%##@%@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@%@##%%%@@@@@%%%%%%%%%%%%@@%%###%%##%@@@@@%%%%%%%%%%@@@@@@@@@@%#%@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@%%%@@@@@@@%%%%%%%%%%%@@@%%*#%@%#%@@@@@%%%%%%@@%%%%%@@@@%%@@%##@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@%##%%%%%###%%@@%%%%%@%%%@@@@@%%@%##%%##%@@@@@@%@@@%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@%%%%%####%@@@%%%@@@%%@@@@%%%@@#+%@@%%@@@@@@@%@@@@%%@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
        system("pause");
        system("cls");
        main();
        break;

    default:
        system("cls");
        main();

    }


}