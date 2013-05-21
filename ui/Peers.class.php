<?php

    class Peers
    {
	
        private $_pID;
        private $_uID;
        private $_pIP;
        private $_pPort;
        private $_device_name;
        private $_device_type;
        
        
        public function __construct ($uID){
        	$this->_uID=$uID;
            $this->_pIP=$this->getIP();
            $this->_pPort=8088;
        }
        
        
        public function getpID(){ return $this->_pID; }
        
        
        public function getIP(){
			//check the user IP
			if (isset($_SERVER['HTTP_X_FORWARDED_FOR'])){
       			return $_SERVER['HTTP_X_FORWARDED_FOR'];
    		}
    		elseif (isset($_SERVER['HTTP_CLIENT_IP'])){
        		return $_SERVER['HTTP_CLIENT_IP'];
    		}
    		else{
      			return $_SERVER['REMOTE_ADDR'];
    		}
		}
		
		//update the Peers table
		public function setIP($device_name){
			try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
			       die('Erreur : ' . $e->getMessage());
			}

			$query = $bdd->prepare('UPDATE Groups SET pIP = :pIP WHERE uID = :uID AND device_name = :device_name');
			$query->execute(array(
				'pIP' => $this->_pIP,
				'uID' => $this->_uID,
				'device_name' => $device_name,
			));
			
			//searching for the pID
			$query = $bdd->prepare('SELECT pID FROM Peers WHERE uID = :uID AND device_name = :device_name');
			$query->execute(array(
				'uID' => $this->_uID,
				'device_name' => $device_name,
			));
			//and store it
			while($data = $query->fetch()){ $this->setpID($data['pID']); }
		}

		public function setpID($pID){ $this->_pID=$pID; }

        //insert the new device into the Peers table
        public function add_device ($device_name, $device_type)
        {
            $this->_device_name=$device_name;
            $this->_device_type=$device_type;

			try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
			       die('Erreur : ' . $e->getMessage());
			}
		
			$query = $bdd->prepare('INSERT INTO Peers VALUES(\'\', :uID, :pIP, :pPort, :device_name, :device_type)');
    		$query->execute(array(
				'uID' => $this->_uID,
				'pIP' => $this->_pIP,
				'pPort' => $this->_pPort,
				'device_name' => $device_name,
				'device_type' => $device_type,
			));
			$this->setpID($bdd->lastInsertId());	//store the pID
		}
		
		//insert into the Uploads table the informations about the upload
		public function upload($vID, $gID){
			try{
				$bdd = new PDO('mysql:host=localhost;dbname=pWeb', 'root', '');
			}catch (Exception $e){
			    die('Erreur : ' . $e->getMessage());
			}
			if(empty($gID)){	//regular upload
				//update the Uploads table
    			$query = $bdd->prepare('INSERT INTO Uploads VALUES(:pID, :vID, "", :date)');
				$query->execute(array(
					':pID' => $this->_pID,
					':vID' => $vID,
					':date' => date(DATE_RFC822),
				));
			}else{	//upload in a group
				//update the Uploads table
    			$query = $bdd->prepare('INSERT INTO Uploads VALUES(:pID, :vID, :gID, :date)');
				$query->execute(array(
					':pID' => $this->_pID,
					':vID' => $vID,
					':gID' => $gID,
					':date' => date(DATE_RFC822),
				));
			}
		}	
		
	}
?>