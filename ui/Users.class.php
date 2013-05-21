<?php

    class Users
    {
	
        private $_uID;
        private $_Username;
        private $_Password;
        private $_last_seen;
        private $_peer;
        private $_groups;
        
        public function __construct ($Username, $Password)
        {
            $this->_Username=$Username;
            $this->_Password=$Password;
            $this->_last_seen=date(DATE_RFC822);
            $this->_groups=array();
        }
        
        //Getters
        public function getuID(){ return $this->_uID; }
        
        public function getUsername(){ return $this->_Username; }
        public function getPassword(){ return $this->_Password; }
        public function getPeer(){ return $this->_peer; }
        
        public function getGroups(){ return $this->_groups; }
        
        public function getGroup($gID){
        	foreach($this->_groups as $group){
        		if(strcmp($group->getgID(), $gID)==0){ return $group; }
        	}
        }

		//return true if the user exists
        public function exists(){
            try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			
			$count=0;
		
        	$query = $bdd->query('SELECT uID, Username, Password FROM Users');

			while ($data = $query->fetch()){	//searching if the user already exists
				if(strcmp($data['Username'], $this->_Username)==0 && strcmp($data['Password'], md5($this->_Password))==0){
					$this->setuID($data['uID']);
					$count++;
				}
			}
		
			if($count==1){	//the user exists
				return true;
    		}else{	//the user doesn't exist
				return false;
			}
        }
        
        public function addPeer(){
        	$this->_peer= new Peers($this->_uID);
        }
        
        public function setuID($uID){
        	$this->_uID=$uID;
        }
                
        //return the result query of searching all the devices by using the uID
        public function getDevices(){
        	try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			
        	//collecting all the user's devices
    		$query = $bdd->prepare('SELECT device_name,device_type FROM Peers P, Users U WHERE P.uID=U.uID AND U.uID = :uID');
    		$query->execute(array(
				'uID' => $this->_uID,
			));
			return $query;
        }
        
        //update the "last seen" attribute of the Users table 
        public function update(){
        	try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			
    		$query = $bdd->prepare('UPDATE Users SET last_seen = :last_seen WHERE uID = :uID');
    		$query->execute(array(
    			'last_seen' => $this->_last_seen,
				'uID' => $this->_uID,
			));
        }
        
		public function usernameExist()
		{
		try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			$q = $bdd->prepare('SELECT COUNT(*) FROM Users WHERE Username = :Username');
            $q->execute(array(':Username' => $this->_Username));
            
            return (bool) $q->fetchColumn();
		}
        //return the number of uIDs similar to the current one
        public function uIDexists(){
            try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			$q = $bdd->prepare('SELECT COUNT(*) FROM Users WHERE uID = :uID');
            $q->execute(array(':uID' => $this->_uID));
            
            return (bool) $q->fetchColumn();
        }
        
        //insert the user informations into the Users table
        public function registration(){
            try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			
        	$query = $bdd->prepare('INSERT INTO Users VALUES(:uID, :Username, :Password, :last_seen)');
			$query->execute(array(
				'uID' => $this->_uID,
				'Username' => $this->_Username,
				'Password' => md5($this->_Password),
				'last_seen' => $this->_last_seen,
			));
        }
        
        //take all the groups in which the current user is registered
        //store them in an array
        public function updateGroupList(){
        	try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			
			$query = $bdd->prepare('SELECT GM.gID, G.Description, G.isPrivate
									FROM Group_Members GM, Groups G
									WHERE uID=:uID AND GM.gID=G.gID;');
			$query->execute(array(
				'uID' => $this->_uID,
			));
			
			while($data = $query->fetch()){
				if($data['isPrivate']==0){ $isPrivate=false; }else{ $isPrivate=true; }
				$group = new Groups($data['gID'], $data['Description'], $isPrivate);
				array_push($this->_groups, $group);
			}
        }
        
        //if the current user is not registered to the $group
        //insert his uID into the Group_Members table
        public function joinGroup($group){
            try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
    			die('Erreur : ' . $e->getMessage());
			}
			
			$count=0;
			foreach($this->_groups as $joinedGroup){
				if(strcmp($group->getgID(), $joinedGroup->getgID())==0) $count++;
			}
			
			if($count>0){ return false;
			}else{
				array_push($this->_groups, $group);

        		$query = $bdd->prepare('INSERT INTO Group_Members VALUES(:gID, :uID)');
				$query->execute(array(
					'gID' => $group->getgID(),
					'uID' => $this->_uID,
				));	
				return true;
			}
        }
    }
    
?>