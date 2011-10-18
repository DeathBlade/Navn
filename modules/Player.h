class Player
{
private:
  Flux::string name;
  int hp;
  int energy;
public:
  Player()
  {
    name = "NoArg";
    hp = 100;
    energy = 100;
  }
  Player(Flux::string n)
  {
    name = n;
    hp = 100;
    energy = 100;
  }

  Flux::string Name() { return name; }
  bool Alive() { if (hp <= 0) return false; else return true; }
  void HurtBy(int x) { hp -= x; }
  int Health() { return hp; }
  int Energy() { return energy;}
  bool Tired() { if (energy <= 0) return true; else return false; }

};