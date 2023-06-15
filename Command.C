Suffix = "..";
keys = list(
	"mim", 
	"trig", 
	"treat", 
	 "pinnedrecipe", 
	"pinrecipe",
	"recipe", 
	 "creator", 
	 "radioPinMsg", 
	 "addShorthand", 
	 "serverinfo",
	 "leaderboard",
	 "help");
desc = list(
			"Allows you to mimick any individual. First argument is the content of the message. Second argument is the name of the person to mimick. Third, their job name. Example: [ "+Suffix+"mim I Am the LAW![quote here]Bob Steele[quote here]Security Officer ]",
			"Allows you to send a remote trigger signal. First argument is the frequency and second is the code. Example: [ "+Suffix+"trig 135.5[quote here]25 ]", 
			"Gives you ways to treat a symptom by providing the symptom to treat. Example: [ "+Suffix+"treat bleeding ]",
			"Shows the ingredients left on the pinned recipe(use pinrecipe command first). To remove an ingredient add the list number as an argument. To remove: [ "+Suffix+"pinnedrecipe 1 ], to view the list: [ "+Suffix+"pinnedrecipe ] ",
			"Pins or replaces a pinned recipe item, allowing you make recipe without forgetting what you have left. Example: [ "+Suffix+"pinrecipe Acetone ]",
			"Gives you a chemical recipe for a given chemical. Example: [ "+Suffix+"recipe Ash ]",
			mem("creator"),
			"Allows Command crew to pin a message, message is announced every X amount of seconds to remind crew members. Example remind crew to eat their vegetables every 5 seconds: [ "+Suffix+"radioPinMsg Eat your vegetables[quote here]5 ]. To remove a pinned message, simply type: [ "+Suffix+"radioPinMsg remove ]. Each command member can only pin one message.", 
			"Allows you to quickly replace a word/sentence or symbol with another word/sentence or symbol, for example if you are being chased and want to call for help by typing [!!], you can set that with: [ "+Suffix+"addShorthand !![quote here]Help! I'm in danger! ]",
			"Gives misc related server info, mainly used by Net Admin.",
			"Shows a leaderboard of messages sent by these players",
			"Sends you the command list. Also accepts one argument, the command you would like to learn more about. Example: [ "+Suffix+"help trig ]");
//1-5 = Brute, 6-9 = burn, 10-16 = Suffocation, 17+ = Toxins
symptom = list("Bleeding", "Bloodied clothing", "Bloodied limbs", "bleeding", "blood",
		"Burning","burn", "husked burn", "burn damage",
		"Gasping", "gasps","air","no ox","no oxygen","passing out", "unconcious",
		"Vomiting","vomit","puking","pukes","throws up","throwing up","puke");

treatments = list("Treatments: Styptic Powder, Synthflesh, Libital (Deals damage to liver) ,Cryoxadone in a cryotube or a very cold environment.",
			"Treatments: Mesh when applied to the damaged limb, Silver sulfadiazine, Synthflesh, Oxandrolone pills (25u overdose threshold), Aiuri (Deals damage to eyes), Cryoxadone in a cryotube or cold place.",
			"Treatments: Epinephrine, Salbutamol, Perfluorodecalin, Cryoxadone in cryotube or cold place, missing heart then replace heart, if heart stopped then use a defibrilator",
			"Treatments: Charcoal, Anti-toxin (30u overdose threshold), Pentetic acid, Calomel, Cryoxadone in a cryotube or very cold place.");


Chemicals = mem("Chems");
ChecicalRecipes = mem("Recipes");
jobList = mem("JobList");
serverData = list(
	"StartUptime" = time(), 
	"MessagesProccessed" = 0, 
	"ActiveServers" = list(), 
	"leaderboardIndex" = list(),
	"leaderboardValue" = list(),
	"LastMessagesSentByusers" = list()
	);
savedServerInfo = mem("ServerInfo");
if(length(savedServerInfo) == 0){
	mem("ServerInfo", serverData);
}else{
	serverData = savedServerInfo;
}
name = "TComm Script";

def customJobName(job, name){
    jobFinder = find(jobList, job);

    if (jobFinder != 0) {
        return jobList[jobFinder];
    }
    return job;
}
def get_args(ofTxt, cmd){
	hasPrefix = find(substr(ofTxt, 1, length(cmd)+1), cmd);
	if(hasPrefix){
		txt = substr(ofTxt, length(cmd)+1, length(ofTxt)+1);
		return explode(txt, "\"") || explode(txt, " ") || list(tostring(txt));
	}
}
def trig(sig, args){
	argamount = length(args);
	content = "Succesfully triggered freq "+args[1]+" with code "+args[2];
	channel = sig.freq;
	if(argamount > 0){
		content = "Did not pass all required arguments. Remember to use quotes in between each argument.";
	}
	NewSign = signal(content, channel, name);
	NewSign.filters = list(filter_types.loud);
	broadcast(NewSign);
	return sig;
}

def help(sig, args){
	channel = sig.freq;
	NeedHelpForCommand = 0;
	index = 1;
	if(length(args) > 0){
		while(index < length(keys)+1){
			cmdName = keys[index];
			hasCommand = find(args[1], cmdName);
			if(hasCommand){
				NeedHelpForCommand = index;
				break;
			}
			index += 1;
		}
	}
	if(NeedHelpForCommand){
		helpDesc = desc[NeedHelpForCommand];
		content = "@"+sig.source+", "+helpDesc;
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}else{
		content = "@"+sig.source+", here are the Available commands, type \""+Suffix+"help [command]\" for more information on that command: "+keys.Join(", ");
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}
}
def mim(sig, args){
	content = args[1];
	if(!args[1]){ content = sig.content; }
	channel = channels.common;
	title   = args[2];
	if(!args[2]){ title = "Michael Jackson"; }
	job     = args[3];
	if(!args[3]){ job = sig.job; }
	newSig = signal(content, channel, title, job);
	newSig.filters = list(filter_types.loud,"yell");
	broadcast(newSig);
	sig.pass = 0;
	return sig;
}
def treatment(sig, args){
	channel = sig.freq;
	hasSymptom = 0;
	if(length(args) > 0){
		//1-5 = Brute, 6-9 = burn, 10-16 = Suffocation, 17+ = Toxins
		symptonIndex = find(args[1], symptoms);
		if (SymptonIndex >= 1 && SymptonIndex <= 5){//Brute
			hasSymptom = 1;
		}
		if (SymptonIndex >= 6 && SymptonIndex <= 9){//burn
			hasSymptom = 2;
		}
		if (SymptonIndex >= 10 && SymptonIndex <= 16){//Suff
			hasSymptom = 3;
		}
		if (SymptonIndex >= 17 ){//Toxins
			hasSymptom = 4;
		}
	}
	if(hasSymptom){
		treatmentNameToIndex = list("Brute", "Burn", "Suffocation", "Toxin");
		content = "@"+sig.source+", "+treatmentNameToIndex[index]+" "+treatments[index];
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}else{
		content = "@"+sig.source+", was not able to process your symptom: ["+args[1]+"]. If you know of the cure please tell your patient to wait and inform the network admin to update the entries, afterall we are no doctors.";
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}
}
def chems(sig, args){
	if (length(args) > 0){
		nospace = substr(args[1], 2, length(args[1]) + 1);
		chemIndex = find(Chemicals, nospace);
		channel = sig.freq;
		if(chemIndex>0){
			content = "@"+sig.source+", Recipe for ["+nospace+"]: "+ChecicalRecipes[chemIndex];
			NewSign = signal(content, channel, name);
			NewSign.filters = list(filter_types.loud);
			broadcast(NewSign);
			return sig;
		}else{
			content = "@"+sig.source+", you did not pass a chemical name ["+nospace+"]. Please use the format provided by the "+Suffix+"help command.";
			NewSign = signal(content, channel, name);
			NewSign.filters = list(filter_types.loud);
			broadcast(NewSign);
			return sig;
		}
	}else{
		content = "@"+sig.source+", was not able to process chemical Request ["+args[1]+"]. Please allow Technician to look at the logs or try a variation of that same chemical name.";
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}
}
//"Shows the ingredients left on the pinned recipe(use pinrecipe command first). To remove an ingredient add the list number as an argument. Example: "+Suffix+"pinned 1",
def pinnedrecipe(sig, args){
	//Get saved pinned recipe
	hasRemoveIndex = 0;
	if (length(args) > 0){ hasRemoveIndex = tonum(args[1]); }
	pinnedRecipe = mem(sig.source+"Pinned");
	channel = sig.freq;
	if(hasRemoveIndex > 0){
		prevShownList = mem(sig.source+"PinnedShownList");
		if(prevShownList == "true" ){
			pinnedRecipe["recipes"].Cut(hasRemoveIndex, hasRemoveIndex);
			finishedString = "";
			index1 = 1;
			while(index1 < length(pinnedRecipe["recipes"])+1){
				finishedString = finishedString+', '+index1+': '+pinnedRecipe["recipes"][index1];
				index1 += 1;
			}
			content = "@"+sig.source+", You've removed recipe at index ["+hasRemoveIndex+"], here's the updated list: "+finishedString;
			mem(sig.source+"PinnedShownList", prevShownList);
			NewSign = signal(content, channel, name);
			NewSign.filters = list(filter_types.loud);
			broadcast(NewSign);
			return sig;
		}else{
			pinnedRecipe["recipes"].Cut(hasRemoveIndex, hasRemoveIndex);
			//pinnedRecipe["recipes"][hasRemoveIndex] = null;
			finishedString = "";
			index2 = 1;
			while(index2 < length(pinnedRecipe["recipes"])+1){
				item = pinnedRecipe["recipes"][index2];
				finishedString = finishedString+', '+index2+': '+item;
				index2 += 1;
			}
			mem(sig.source+"PinnedShownList", "true");
			mem(sig.source+"Pinned", pinnedRecipe);

			content = "@"+sig.source+", You have not checked which recipe you are removing, it's your loss. Removing recipe at Index ["+hasRemoveIndex+"], what is left: "+finishedString;
			NewSign = signal(content, channel, name);
			NewSign.filters = list(filter_types.loud);
			broadcast(NewSign);
			return sig;
		}
	}
	if(length(pinnedRecipe) == 2){
		finishedString = "";
		index3 = 1;
		while(index3 < length(pinnedRecipe["recipes"])+1){
			item = pinnedRecipe["recipes"][index3];
			finishedString = finishedString+', '+index3+': '+item;
			index3 += 1;
		}
		mem(sig.source+"PinnedShownList", "true");
		mem(sig.source+"Pinned", pinnedRecipe);
		content = "@"+sig.source+", Recipes left: "+finishedString;
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}else{
		content = "@"+sig.source+", No recipe has been pinned, please use command ["+Suffix+"help pinrecipe] for more info on pinning recipes.";
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}
	return sig;
}
def pinrecipe(sig, args){
	channel = sig.freq;
	recipeName = substr(args[1], 2, length(args[1]) + 1);
	chemIndex = find(Chemicals, recipeName);
	if(chemIndex == 0 ){
		content = "@"+sig.source+", Could not find a recipe for chemical name ["+recipeName+"], please check your spelling. ";
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}
	recipeString = ChecicalRecipes[chemIndex];
	pinData = list("recipes" = explode(recipeString, ","), "recipeName" = recipeName);
	mem(sig.source+"Pinned", pinData);
	content = "@"+sig.source+", Succesfully pinned recipe ["+recipeName+"].["+pinData["recipes"]+"]";
	NewSign = signal(content, channel, name);
	NewSign.filters = list(filter_types.loud);
	broadcast(NewSign);
	return sig;
}
def radioPinMsg(sig, args) {
	channel = sig.freq;
	CommandPins = mem("CommandsPinnedBy") || list();
	Command = list("Keeper of Nukes", "Head of Greytiders", "Head of Security", "Master Plasma Leaker", "Ai Wrangler", "Dr. Faust", "Network Admin", "Station Central Processor");
	//Make sure it's a Command Crew memeber
	if(find(Command,  sig.job) == 0){
		content = "@"+sig.source+", Sorry this command is restricted only to crew members in Command. Ask them if they can pin the message for you.";
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig; //Not a Command crew, exit.
	}
	UserPinnedMessage = mem(sig.source+"RadioPin");
	//Make sure there are args provided
	if(length(args) == 0){
		HasPinned = length(UserPinnedMessage) > 0;
		ending  = ".";
		if(HasPinned){
			ending = " or the remove message.";
		}else{
			ending = ".";
		}
		content = "@"+sig.source+", You did not provide the Message"+ ending;
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig; //No arguments passed.
	}
	MessageOrRemove = args[1];
	Amount = tonum(args[2]) || 3;
	//Check if it's a remove command
	if(MessageOrRemove == "remove" || MessageOrRemove == "Remove" || MessageOrRemove == " remove"){
		mem(sig.source+"RadioPin", null);
		content = "@"+sig.source+", Pinned Message was removed!";
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}
	//Add the pinned message
	SavePin = list("Msg" = MessageOrRemove, "length" = Amount, "Stamp" = time());
	mem(sig.source+"RadioPin", SavePin);
	isAdded = find(CommandPins, sig.source);
	if( isAdded == 0){
		CommandPins += sig.source;
	}
	content = "@"+sig.source+", Pinned Message ["+SavePin["Msg"]+"], announced every ["+SavePin["length"]+"] seconds";
	NewSign = signal(content, channel, name);
	NewSign.filters = list(filter_types.loud);
	broadcast(NewSign);
	mem("CommandsPinnedBy", CommandPins);
}
def addShorthand(sig, args){
	channel = sig.freq;
	currentShorthands = mem(sig.source+"ShortHands") || list();
	ShorthandKey = args[1];
	ShorthandVal = args[2];
	if (length(ShorthandKey) == 0){
		content = "@"+sig.source+", You did not provide the first value, it's used to replace it with argument 2.";
		NewSign = signal(content, channel, name);
		NewSign.filters = list(filter_types.loud);
		broadcast(NewSign);
		return sig;
	}
	currentShorthands[ShorthandKey] = list("key" = ShorthandKey, "value" = ShorthandVal);
	if (length(ShorthandVal) == 0){
		currentShorthands[ShorthandKey] = list("key" = ShorthandKey, "value" = ShorthandKey);
	}
	mem(sig.source+"ShortHands", currentShorthands);
}
//serverData = list("StartUptime" = time(), "MessagesProccessed" = 0, "ActiveServers" = list(); "LastMessagesSentByusers" = list());
def serverinfo(sig, args) {
	channel = sig.freq;
	ServerData = mem("ServerInfo");
	timearg = args[1];
	timeset = args[2];
	startUpT = ServerData["StartUptime"];
	msgProc  = ServerData["MessagesProccessed"];
	actServ  = ServerData["ActiveServers"];
	lastUsrMsgs = ServerData["LastMessagesSentByusers"];
	current = time();
	if(timearg == "time" || timearg == " time"){
		lastTime = ServerData["LastUpdate"] || 0;
		if(lastTime > 0){
			differe = (current - lastTime) / 10;
			remainder = differe % 3600;
			minsDiff = floor(remainder / 60);
			secsDiff = (remainder % 60);
			content = "@"+sig.source+", Server Info... time since last set ["+lastTime+"] current ["+tostring(current)+"] difference["+tostring(differe)+"] remain["+tostring(remainder)+"] time passed since message update min["+tostring(minsDiff)+"] sec["+tostring(secsDiff)+"]";
			NewSign = signal(content, channel, name);
			NewSign.filters = list(filter_types.loud);
			broadcast(NewSign);
		}else{
			content = "@"+sig.source+", Server Info... time since last set [NOT SET] current ["+tostring(time())+"]";
			NewSign = signal(content, channel, name);
			NewSign.filters = list(filter_types.loud);
			broadcast(NewSign);
		}
		if( length(timeset) > 0){
			ServerData["LastUpdate"] = time();
			mem("ServerInfo", serverData);
		}
		return sig;
	}
	userString = "";
	serverString = "";
	msgindex = 1;
	servIndex = 1;
	while(msgindex < length(lastUsrMsgs) + 1) {
		packet = lastUsrMsgs[msgindex];
		if( length(packet) > 0){ 
			if(msgindex == 1){
				userString += packet;	
			}else{
				userString += ", "+packet;
			}
		}
		msgindex += 1;
	}
	while(servIndex < length(actServ) + 1) {
		freq = actServ[servIndex];
		if(length(freq) == 0){ continue; }
		if( servIndex == 1 ) {
			serverString += freq;
		}else{
			serverString += ", "+freq;
		}
		servIndex += 1;
	}
	differe = (current - startUpT) / 10;
	remainder = differe % 3600;
	minsDiff = floor(remainder / 60);
	secsDiff = (remainder % 60);
	content = "@"+sig.source+", Server Info... Start up time - ["+tostring(startUpT)+"]= ["+tostring(minsDiff)+" minutes, "+secsDiff+" Seconds Ago] Messages Proccessed ["+tostring(msgProc)+"] Active Channels: ["+serverString+"] Last three msgs sent by users: ["+userString+"]";
	NewSign = signal(content, channel, name);
	NewSign.filters = list(filter_types.loud);
	broadcast(NewSign);
}

def process_signal(sig){
	channel = sig.freq;
	// Fuck your ramblings, poly
    if (sig.source == "Poly" || sig.source == "The Ghost of Poly") {
        return;
    }
    serverData["MessagesProccessed"] += 1;
	if(find(serverData["ActiveServers"], tostring(channel)) == 0){
		serverData["ActiveServers"] += tostring(channel);
	}
	if(length(serverData["LastMessagesSentByusers"]) > 0){
		serverData["LastMessagesSentByusers"] += list(sig.source);	
	}else{
		serverData["LastMessagesSentByusers"] = list(sig.source);	
	}
	playerFoundInleader = find(serverData["leaderboardIndex"], sig.source);
	if(playerFoundInleader == 0){
		serverData["leaderboardIndex"] += sig.source;
		serverData["leaderboardValue"] += 1;
	}else{
		serverData["leaderboardValue"][playerFoundInleader] += 1;
	}
	
	if( length(serverData["LastMessagesSentByusers"]) > 3 ){
		newCopy = serverData["LastMessagesSentByusers"].Copy(length(serverData["LastMessagesSentByusers"]) - 3, length(serverData["LastMessagesSentByusers"])); 
		serverData["LastMessagesSentByusers"] = newCopy;
	}
	mem("ServerInfo", serverData);
    CommandersWhoPinned = mem("CommandsPinnedBy") || list();
    sig.job = customJobName(sig.job, sig.source);
    sig.source += " [" + sig.job + "]";
    hasShorthands = mem(sig.source+"ShortHands");
    amountShortha = length(hasShorthands);
    if( amountShortha > 0){
    	shindex = 1;
    	while( shindex < amountShortha + 1 ){
    		handToReplace = hasShorthands[shindex]["key"];
    		replaceWith   = hasShorthands[shindex]["value"];
    		sig.content.Replace(handToReplace, replaceWith);
    	}
    }
	commands = list(
		"mim" = mim, 
		"trig" = trig, 
		"treat" = treatment, 
		"pinnedrecipe" = pinnedrecipe, 
		"pinrecipe" = pinrecipe, 
		"recipe" = chems, 
		"radioPinMsg" = radioPinMsg, 
		"addShorthand" = addShorthand, 
		"help" = help,
		"leaderboard" = mem("leaderboardDef"),
		"serverinfo" = serverinfo
		);
	if ( length(CommandersWhoPinned) > 0 ){
		PinIndex = 1;
		while( PinIndex < length(CommandersWhoPinned) + 1){
			source = CommandersWhoPinned[PinIndex];
			GetPinData = mem(source+"RadioPin");
			if (length(GetPinData) == 0 ){ continue; }
			if (GetPinData["Stamp"] == NULL){ continue; }
			if (GetPinData["length"] == NULL){ continue; } 
			current = time();
			savedtime = GetPinData["Stamp"];
			currentLength = GetPinData["length"];
			secondsSinceStamp = (current - savedtime) / 10;
			if ( secondsSinceStamp > currentLength ){
				content = " --- PINNED -- "+ GetPinData["Msg"];
				noname = "";
				NewSign = signal(content, channel, noname);
				NewSign.filters = list(filter_types.loud);
				broadcast(NewSign);
				GetPinData["Stamp"] = time();
				mem(source+"RadioPin", GetPinData);
			}
			PinIndex += 1;
		}
	}
	mainIndex = 1;
	while(mainIndex < length(keys)+1){
		content = sig.content;
		cmdName = keys[mainIndex];
		cmdWithSuffix = Suffix+cmdName;
		txt = substr(content, 1, length(cmdWithSuffix)+1);
		hasCommand = find(txt, cmdWithSuffix);
		args = get_args(content, cmdWithSuffix);
		if(hasCommand){
			commands[cmdName](sig, args, cmdName);
			return sig;
		}
		mainIndex += 1;
	}
	return sig;
};

//Radio commands available to everyone! Type "..help" for more info!
