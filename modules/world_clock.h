/* All code is licensed under GNU General Public License GPL v3 (http://www.gnu.org/licenses/gpl.html) */
#define MST (-6)
#define UTC (0)
#define CCT (+8)
#define EST (-4)
#define PST (-7)
#define AUS (+10)
#include "flux_net_irc.hpp"
int days, hours, mins;

/**
 * \file world_clock.h Header file holding the \a world_clock function.
 * \author Lordofsraam. Polished by Justasic.
 */

/**
 * \defgroup world_clockM World Clock Module
 * This is the module for the world clock function.
 * For a better description see the function description.
 * \section commands Commands associated with this module.
 * \subsection time !time
 * Say \a !time to see the current time, the GMT, and the times in New York, California, Beijing, and Sydney.
 * @{
 */

/**
 * \fn class world_clock(bool a):module("World Clock", a, PRIORITY_DONTCARE){ this->SetDesc("Shows time for your area or for the preset ones"); }
 * \brief Tells the world time.
 * Uses C++ time functions to tell the time around the world.
 */

class world_clock:public module{
public:
  world_clock():module("World Clock", PRIORITY_DONTCARE){ 
    this->SetAuthor("Lordofsraam");
    ModuleHandler::Attach(I_OnPrivmsg, this);
  }
  void OnPrivmsg(User *u, Channel *c, std::vector<Flux::string> &params){
  Flux::string cmd = params.empty()?"":params[0];
  
  Flux::string msg;
    for(unsigned i=0; i < params.size(); ++i){
      msg += params[i];
      msg.AddSpace();
    }
  
  if (cmd.equals_ci("!time")){
    Flux::string location;
    if(params.size() > 2)
      location = params[params.size() -1];
    else
      location = params.size() == 2?params[1]:"";
    
    if(location.empty()){
      time_t rawtime;
      tm * ptm;
      time(&rawtime);
      ptm = gmtime(&rawtime);
      int minutes = (ptm->tm_min);
      int seconds = (ptm->tm_sec);
      c->SendMessage("Current time around the World:");
      c->SendMessage("GMT == %2d:%02d:%02d", (ptm->tm_hour+UTC)%24, minutes, seconds);
      c->SendMessage("New York (USA) == %2d:%02d:%02d", (ptm->tm_hour+EST)%24, minutes, seconds);
      c->SendMessage("California (USA) == %2d:%02d:%02d", (ptm->tm_hour+PST)%24, minutes, seconds);
      c->SendMessage("Beijing (China) == %2d:%02d:%02d", (ptm->tm_hour+CCT)%24, minutes, seconds);
      c->SendMessage("Sydney (Australia) == %2d:%02d:%02d", (ptm->tm_hour+AUS)%24, minutes, seconds);	
      char buf[100];
      ptm = localtime(&rawtime);
      strftime(buf,100,"Navn's Time: %Z %c",ptm);
      c->SendMessage(buf);
      log(LOG_NORMAL, "%s requested !time command in %s", u->nick.c_str(), c->name.c_str());
      return;
    }else{
      Flux::string wget, tmpfile = TempFile(binary_dir+"/runtime/navn_xml.tmp.XXXXXX");
      	if(location.is_number_only())
	  wget = "wget -q -O "+tmpfile+" - http://www.google.com/ig/api?weather="+location;
	else
	  wget = "wget -q -O "+tmpfile+" - http://www.google.com/ig/api?weather="+urlify(removeCommand("!time", msg));
	system(wget.c_str());
	if(!irc_string::said(xmlToString(tmpfile),"problem_cause")){
	  Flux::string ff = xmlToString(tmpfile);
	  ff.trim();
	  if(ff.empty()){
	   c->SendMessage("Could not download/read %s", tmpfile.c_str());
	   log(LOG_NORMAL, "%s attempted to use !time but downloading/reading the file '%s' failed.", tmpfile.c_str());
	   return;
	  }
	  Flux::string loc = findInXML("city","data",ff);
	  Flux::string time = findInXML("current_date_time","data",ff);
	  c->SendMessage("The current time in %s is %s", loc.c_str(), time.c_str());
	  Delete(tmpfile.c_str());
	  log(LOG_NORMAL, "%s used !time to get time for %s", u->nick.c_str(), location.c_str());
	  }
	}
      }
    }
};
MODULE_HOOK(world_clock)
/**
 * @}
 */

