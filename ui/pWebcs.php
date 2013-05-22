<?php

require 'Peers.class.php';
require 'Users.class.php';


define("maxPatternLength", 36);

session_start();

if(isset($_POST['home_agent'])){
        $_SESSION['homeagent'] = $_POST['home_agent'];
}
?>

<!doctype html>
<html lang="en">
<head>
	<title>pWeb</title>
	<link rel="stylesheet" href="styles.css" type="text/css" media="screen" />
	
	
<script src="js/jquery.js" type="text/javascript"></script>
<script type="text/javascript" src="js/equalcolumns.js"></script> 
 <script src="js/jquery-1.9.1.js"></script>
  <script src="js/jquery-ui.js"></script>
  <link rel="stylesheet" href="jquery-ui.css" />

	
	
	
</head>
<body>

<script type="text/javascript"> 

function build_ha_name(homeagent){
	server_port = "20005";
	ha_suffix = ".dht.pwebproject.net";
	//ha_suffix = ".cs.uwaterloo.ca";
	//ha_suffix = "";
	homeagent = "" + homeagent;
	//alert(homeagent);
	return "http:\/\/" + homeagent + ha_suffix + ":" + server_port + "\/"; 
}

function getha() {
   var tagss=[];
   
   $.ajax({
                url: 'getHA.php',
                type: 'POST', 
                data: $(this).serialize(), 
                dataType: 'json',
                success: function(json) {
				tagss=json.ha;
				 $( "#home_agent" ).autocomplete({
      source: function( request, response ) {
    var matches = $.map( tagss, function(tag) {
      if ( tag.toUpperCase().indexOf(request.term.toUpperCase()) === 0 ) {
        return tag;
      }
    });
    response(matches);
  }
    });
				
				}
				});
   
	

   
  }


function cancelMod()
{
							  window.location.href='pWebcs.php?fn=device_list';

}

function deleteitem(name)
{

var homeagent =  '<?php if (isset ($_SESSION['homeagent'])) echo $_SESSION['homeagent']; ?>';
//var ha=homeagent+'.pWebproject.net';
var ha=build_ha_name(homeagent);

$.ajax({
                //url: 'fichier.php', //ha+'/?method=deleteDevice&name='+name,
		url: ha+'?method=deleteDevice&name='+name,
                type: 'GET', 
                success: function(json) {
                    if (json.status  == 'success')
					{
				
					location.assign(location.href);
					}
					}
					});
}


function modifyitem(name,port,publicf,privatef)
{

document.modifyDev.device_name.value = name;
document.modifyDev.new_device_name.value = name;
document.modifyDev.device_port.value = port;
document.modifyDev.public_folder.value = publicf;
document.modifyDev.private_folder.value = privatef;

$('#listDevice').hide();

$('#modifDev').show();

}



function listDev()
{

	    var homeagent=  '<?php if (isset ($_SESSION['homeagent'])) echo $_SESSION['homeagent']; ?>';
		var username= '<?php if (isset ($_SESSION['user'])) echo $_SESSION['user']->getUsername(); ?>';
		//var ha=homeagent+'.pWebproject.net';
		var ha = build_ha_name(homeagent);

		
$('#col1').html( '<tr><td><b>device name</b></td><td width="50px"></td><td><b>device type</b></td><td width="50px"></td><td><b>delete</b></td><td width="50px"></td><td><b>modify</b></td></tr>');
		$.ajax({
                //url: 'fichier.php?homeagent='+ha+'&username='+username, //ha+'/?method=getDeviceList&username='+username,
		url: ha+'?method=getDeviceList&username='+username,
                type: 'GET', 
                success: function(json) {
				$.each(json.devices, function(i,name) {
				device_name=json.devices[i].name;
				port=json.devices[i].port;
				public_folder=json.devices[i].public_folder;
				private_folder=json.devices[i].private_folder;
				name=json.devices[i].name+"."+username;
								$('#col1').append( '<tr><td>'+json.devices[i].name+'</td><td></td><td>'+json.devices[i].type+'</td><td></td><td><a href="javascript:;" id="deleteDev" onClick="deleteitem(\''+name+'\');">delete</a></td><td></td><td><a href="javascript:;" id="modifyDev" onClick="modifyitem(\''+device_name+'\',\''+port+'\',\''+public_folder+'\',\''+private_folder+'\');">modify</a></td></tr>');
								
								});
							
				
				}
				});
				
				
		}


</script>
<script type="text/javascript">
$(document).ready(function() {


$('#selectDev').hide();
	
			 
			 
    $('#authentificationForm').on('submit', function() {
 
       
         var Username = $('#Username').val();
         var Password = $('#Password').val();
	 var homeagent =  '<?php if (isset ($_SESSION['homeagent'])) echo $_SESSION['homeagent']; ?>';
	//var ha=homeagent+'.pWebproject.net';
	var ha=build_ha_name(homeagent);

     
        if(Username == '' || Password == '') {
		    $('#auth').html('<b>Filling all the inputs is required </b>');
           
        } else {
            $.ajax({
                //url: 'fichier.php?homeagent='+ha+'&name='+Username+'&password='+Password, //ha+'/?method=authenticate&username='+Username+'&password='+Password
		url: ha+'?method=authenticate&name='+Username+'&password='+Password,
                type: 'GET', 
                success: function(json) {
                    if (json.status == 'success')
					{
					    
					
					
					$.ajax({
                url: 'pWebcs.php?fn=check_login&username='+Username+'&password='+Password, 
                type: 'POST', 
                data: $(this).serialize(), 
           
										 
										 success: function()
										 {
										 	  window.location.href='pWebcs.php?fn=index';
										 }
				
					
            });
					
					}
                        
						else
						 $('#auth').html('<b>Wrong username or password</b>');
                    
                }
            });
        }
        return false; 
    });
	
		$('#registerForm').on('submit', function() 
	{
	
 
		
		var u = $('#uname').val();
        
		var p1= $('#p1').val();
		var p2= $('#p2').val();
		var uid= $('#uid').val();
		var Full_name= $('#Full_name').val();
		var Country= $('#Country').val();
		var Affiliation= $('#Affiliation').val();
	    	var homeagent='<?php if (isset ($_SESSION['homeagent'])) echo $_SESSION['homeagent']; ?>';
		//var ha=homeagent+'.pWebproject.net';
		var ha = build_ha_name(homeagent);

		
		var text='<font color="red"><b>This account already exists, please change your username!</b></font></br> you can choose from this list: </br> (';

		if ( u =='' || p1 =='' || p2 =='' || uid =='' || Full_name ==''|| Country == '' || Affiliation == '')
					{$('#lp3').html('');
						$('#lp0').html(' <b>Filling all the compulsary inputs is required </b>');
					}
					else if (p1 != p2)
					{   $('#lp0').html('');
					    $('#lp3').html( '<b>Password does not match confirmation</b>');
				
					}
					else
					{
					
					//alert(ha+'?method=existUsername&name='+u);	
					//check availability
					$.ajax({
					
				
                //url: 'fichier.php?username='+ u +'&password='+ p1,  // ha+'/?method=existUsername&name='+u,
                url: ha+'?method=existUsername&name='+u,
		type: 'GET',
		success: function(json)	{
				//alert(json);
				if (json.availability == 'yes')
				    { //registration
					
						$.ajax({
					
					
						//url:'fichier.php?username='+ u +'&password='+ p1,  // ha+'/?method=registerUser&name='+u+'&password='+p1+'&email='+uid+'&Full_name='+Full_name+'&Country='+Country+'&Affiliation='+Affiliation,
						url: ha+'?method=registerUser&name='+u+'&password='+p1+'&email='+uid+'&fullname='+Full_name+'&location='+Country+'&affiliation='+Affiliation,
						type: 'GET', 
						success: function(json)
						{
							  if (json.status == 'success')
							  {
							  
									$.ajax({
					                   //save username in SESSION variable
                                         url: 'pWebcs.php?fn=check_login&username='+u+'&password='+p1,   
                                         type: 'POST', 
                                         data: $(this).serialize(), 
								
										 success: function()
										 {
										 
										 	window.location.href='pWebcs.php?fn=index';
										 }
				
				});
				
			
				
							  }
				
						}
				        });
					
					
					}
					else 
					{ text+=json.suggestion[0];
						$.each(json.suggestion, function(i) {
						if (i>0)
						{
						        text+=' , ';
								text+=json.suggestion[i];
								
								}
								});
								text+=' )';
								$('#suggest').html(text);
								
					}
				},
				error:function(jqXHR, textStatus, errorThrown){
					alert("error in ajax call: " + jqXHR.responseXML + " - " + textStatus + " - " + errorThrown);
				}
								
									
					
					
						});
                    
					    
								
					
                }
            
	return false; 
	});
	
	
	
       $('#modifDev').hide();

		$('#modifyDev').on('submit', function() {
		
        
		var device_name= $('#device_name').val();
		var new_device_name= $('#new_device_name').val();
		var device_port= $('#device_port').val();
		var public_folder= $('#public_folder').val();
		var private_folder= $('#private_folder').val();
		
		if ( device_name =='' ||device_port=='' || public_folder=='' || private_folder=='' || new_device_name =='')
		$('#lp0').html(' <font color="red"><b>Filling all the compulsary inputs is required </b></font>');
		
		else 
		{
		var homeagent =  '<?php if (isset ($_SESSION['homeagent'])) echo $_SESSION['homeagent']; ?>';
		//var ha=homeagent+'.pWebproject.net';
		var ha=build_ha_name(homeagent);

		var username= '<?php if (isset ($_SESSION['user'])) echo $_SESSION['user']->getUsername(); ?>';
		var text='<font color="red"><b>This device name already exists, please change it!</b></font></br> you can choose from this list: </br> (';

		//old_device=device_name+'.'+username;
		//device = new_device_name+'.'+username;
		if (device_name != new_device_name)
		{
		//check availability of new name
		
		

			$.ajax({
                //url: 'fichier.php?homeagent='+ha+'&username='+username, //ha+'/?method=isavailable&name='+device,
                url: ha+'?method=isavailable&devicename='+new_device_name+'&username='+username,
                type: 'GET', 
                success: function(json) {
                    if (json.availability == 'yes')
					 {
					
					 
					 $.ajax({
					
						
						//url: 'fichier.php?homeagent='+ha+'&username='+username,  // ha+'/ ?method=modifyDevice&Oldname='+old_device+'&newName='+device+'&port='+device_port+'publicFolder='+public_folder+'&privateFolder='+private_folder,
						url: ha+'?method=modifyDevice&username='+username+'&devicename='+device_name+'&newdevicename='+new_device_name+'&port='+device_port+'&public_folder='+public_folder+'&private_folder='+private_folder,
						type: 'GET',                       
						success: function(json)
						{
							  if (json.status == 'success')
							  {
							  
							  window.location.href='pWebcs.php?fn=device_list';
							  }
					    }
							  });
							  
		             
					 }
					 else
					 {
					 text+=json.suggestion[0];
						$.each(json.suggestion, function(i) {
						if (i>0)
						{
						        text+=' , ';
								text+=json.suggestion[i];
								
								}
								});
								text+=' )';
								$('#suggest').html(text);
					 
					 }
					 }
					 });
		   }
		   else
		   {
		
			 $.ajax({
					
						
						//url: 'fichier.php?homeagent='+ha+'&username='+username,  // ha+'/ ?method=modifyDevice&Oldname='+old_device+'&newName='+device+'&port='+device_port+'publicFolder='+public_folder+'&privateFolder='+private_folder,
						url: ha+'?method=modifyDevice&username='+username+'&devicename='+device_name+'&newdevicename='+new_device_name+'&port='+device_port+'&public_folder='+public_folder+'&private_folder='+private_folder,
						type: 'GET',                       
						success: function(json)
						{
							  if (json.status == 'success')
							  {
							 
							  window.location.href='pWebcs.php?fn=device_list';
							  }
					    }
							  });
		}
		
		}
		return false;
		});
		
	


		
$('#addDev').on('submit', function() {

		var device_type = $('#device_type').val();
		var device_name= $('#device_name').val();
		var device_port= $('#device_port').val();
		var public_folder= $('#public_folder').val();
		var private_folder= $('#private_folder').val();
		var os= $('#os').val();
		var description= $('#description').val();
		var checked=$('input[name=index]:checked', '#addDev').val();
		
		
		
		
		var text='<font color="red"><b>This device name already exists, please change it!</b></font></br> you can choose from this list: </br> (';

		if (device_type == '' || device_name =='' ||device_port=='' || public_folder=='' || private_folder==''|| os==''|| description=='')
		$('#lp0').html(' <font color="red"><b>Filling all the compulsary inputs is required </b></font>');
		
		else 
		{
		var homeagent =  '<?php if (isset ($_SESSION['homeagent'])) echo $_SESSION['homeagent']; ?>';
		//var ha=homeagent+'.pWebproject.net';
		var ha=build_ha_name(homeagent);

		var username= '<?php if (isset ($_SESSION['user'])) echo $_SESSION['user']->getUsername(); ?>';
		device = device_name+'.'+username;

			$.ajax({
                //url: 'fichier.php?homeagent='+ha+'&username='+username, //ha+'/?method=isavailable&name='+device,
                url: ha+'?method=isavailable&username='+username+'&devicename='+device_name,
                type: 'GET', 
                success: function(json) {
                    if (json.availability  == 'yes')
					{
				     	$.ajax({
					
						
						//url: 'fichier.php?homeagent='+ha+'&username='+username,  // ha+'/?method=register&name='+device+'&port='+device_port+'&type='+device_type+'&os='+os+'&description='+description+'&ispublicly_indexed='+checked,
						url: ha+'?method=register&name='+device+'&port='+device_port+'&type='+device_type+'&public_folder='+public_folder+'&private_folder='+private_folder+'&os='+os+'&description='+description+'&ispublicly_indexed='+checked,			
						type: 'GET', 
						success: function(json)
						{
							  if (json.status == 'success')
							  {
							  window.location.href='pWebcs.php?fn=device_list';
							  }
					    }
							  });
					
					}
					else
					{
					text+=json.suggestion[0];
						$.each(json.suggestion, function(i) {
						if (i>0)
						{
						        text+=' , ';
								text+=json.suggestion[i];
								
								}
								});
								text+=' )';
								$('#suggest').html(text);
								
					}
					}
					});
					
		}
		return false;

});

});

</script>


	<header>
		<h1>pWeb</h1>
	</header>
	<nav>
		<ul>
			<li class="selected"><a href="pWebcs.php?fn=index">Home</a></li>
			 <?php
        	if(isset($_SESSION['connect'])){
			
			echo '<li><a href="pWebcs.php?fn=logout">Logout</a></li>';
			echo '<li><a href="pWebcs.php?fn=device_add">Add device</a></li>';
			echo '<li><a href="pWebcs.php?fn=device_list" id="mydevices">my devices</a></li>';
			echo '<li><a href="#">' . $_SESSION['user']->getUserName() . '@' . $_SESSION['homeagent'] . '</a></li>';
			} else {
			
			echo '<li><a href="pWebcs.php?fn=login">Login</a></li>';
			echo '<li><a href="pWebcs.php?fn=registration">Register</a></li>';
			echo '<li><a href="#">' . '@' . $_SESSION['homeagent'] . '</a></li>';
			} 
			?>
		</ul>
	</nav>
	<section id="intro">
			
	
<?php


//put the SESSION variable in a local one
if(isset($_SESSION['user'])){ $user = $_SESSION['user']; }
?>


<?php




switch($_GET['fn']){

//----------------------------------------------------LOGIN-----------------------------------------------------------------

	
	case "login":
		if(isset($_SESSION['connect'])){
			echo "Your are already logged";
		}else{
		
		?>
		
		
		<center>
			 <div id="content2">
		<header>
			<h3>Log In</h3></header>
		<br/>
		
		
		<?php
		
		if(!isset($_POST['home_agent']) || $_POST['home_agent']=='' )
			{
			?>	
			 <script>
			 window.onload=getha;
			 
  </script>

  
			<center>
			 <div id="content2">
			
			<FORM Method="POST" Action="pWebcs.php?fn=login">
						 <div class="ui-widget">
  <label for="home_agent"><p>Select a home agent:</p> </label>
  <input id="home_agent" name= "home_agent"/>
</div>
			<br/>
			<INPUT type=submit name="ok" value="  ok   ">
			</FORM>
			</div>
			</center>
			<?php
			}
			else{
			// echo $_POST['homeagent'];
			$_SESSION['homeagent']=$_POST['home_agent'];
			
			
			?>
			
		
		<FORM id="authentificationForm" Method="POST" Action="pWebcs.php?fn=check_login">
		    

		
			
			<div id="authform">
			<font color="red"><div id="auth"></div></font><br/>
			<label for="Username">Username :</label><input type="text" name="Username" id="Username" /><br />
			<label for="Password">Password :</label><input type="password" name="Password" id="Password" /><br />
			
			<INPUT type=submit name="Login" value=Login >
			
		</FORM>
		<br/>
		<br/>
		
         do not have account?  <a href="pWebcs.php?fn=registration">Registration</a>
		
		
		
		 <div id="selectDev">
		 select your device:
				</br>
					<FORM id="selectDevice" Method="POST" Action="pWebcs.php?fn=check_login">
						<SELECT name="device" id='dev' >
						<option value="">-- Devices --</option>
						</SELECT>
						<INPUT type=submit name="go" value="Go"><br/>
						
					</FORM>

				
				
									<a href="pWebcs.php?fn=device_add" >Add new Device?</a><br/>
									
		
		 </div>
		 </div>
		 </center>
		  
		<?php
		
		}
		}
		
		
		
	break;
		
//----------------------------------------------------LOGOUT-----------------------------------------------------------------

	case "logout":
		$_SESSION=array();	//empty the session variable
		session_destroy();	//delete the session
		

		echo '<script language="Javascript">
        <!--
        document.location.replace("pWebcs.php?fn=index");
        // -->
        </script>';
		
	break;
	
	


//----------------------------------------------------DEVICE_LIST-----------------------------------------------------------------
case "device_list":

		
 
				?>
				<script>
                window.onload=listDev;
                </script>
				
				
	<div id="listDevice">
    <center><table  bgcolor="#FFFFFF"><div id="col1"></div></table></center></div>

    <div id="modifDev">
	<div id="lp0"></div>
	
		<FORM name="modifyDev" id="modifyDev" Method="POST" Action="fichier.php?homeagent=<?php echo $_SESSION['homeagent'] ?>&username=<?php echo $user->getUsername() ?>">
			
			
			<label>Actual name of your device :</label><input type="text" name="device_name"  id="device_name"  /><br />
			<label>New Name :</label><input type="text" name="new_device_name"  id="new_device_name"  /><br />
								<font color="green"><div id="suggest"></div></font>

			<label>Device Port : </label><input type="text" name="device_port" id="device_port"  /> <br/>
			<label>public folder:</label><input type="text" name="public_folder" id="public_folder" /> <br/>
			<label>private Folder:</label>  <input type="text" name="private_folder" id="private_folder"  /> <br/><br/>
			<center><INPUT type=submit name="modify_device" id="modify_device" value="save modification" >
			<INPUT type=button name="cancel" id="cancel" value="cancel" onClick="cancelMod();"></center>
			
		</FORM>
		
		</div>
		
	<?php
break;	

//----------------------------------------------------CHECK_LOGIN-----------------------------------------------------------------
	
    case "check_login":
		
			
	$user = new Users($_GET['username'], $_GET['password']);
	$_SESSION['connect']=true;

			
	break;
	
		
	
//----------------------------------------------------REGISTRATION-----------------------------------------------------------------
		
	case "registration":
		if(isset($_SESSION['connect'])){
			echo "You have already an account";
		}else{ //registration
		
			 	
			if(!isset($_POST['home_agent']) || $_POST['home_agent']=='')
			{
			?>	
			 <script>
			 window.onload=getha;
			 </script>
			 <center>
<header>
			<h3>Registration Form</h3></header>
			
			 <div id="content2">
			
			<FORM Method="POST" Action="pWebcs.php?fn=registration">
			<div class="ui-widget">
			<label for="home_agent"><p>Select a home agent:</p> </label>
			<input id="home_agent" name= "home_agent"/>
			</div>
			<br/>
			<INPUT type=submit name="ok" value="  ok   ">
			</FORM>
			</div>
			</center>
			<?php
			}
			else{
				$_SESSION['homeagent']=$_POST['home_agent'];
			?>
				
				<center>
			
				 <header>
			<h3>Registration Form</h3></header>
				 <div id="content2">
				<div id="registration">
				<p>	You are registering at home agent <h3><?php echo $_SESSION['homeagent'];?></h3><br />
					Please complete the form below. Mondatory fields are marked with <font color="red">*</font>
				</p>
				
				<FORM id="registerForm" Method="POST" Action="fichier.php?homeagent=<?php echo $_SESSION['homeagent'] ?> " >
				
				<font color="red"><div id ="lp0" ></div></font>
				<label for="Full_name">Full name : <font color="red">* </font></label><input type="text" id="Full_name" name="Full_name" /><br/>
					<label for="Username">Username : <font color="red">* </font></label><input type="text" id="uname" name="Username" /><br/>
					<font color="green"><div id="suggest"></div></font>
					<label for="Password">Password : <font color="red">* </font></label><input type="password" id="p1" name="Password" /><br/>
					<label  for="confirmPassword">confirm password : <font color="red">* </font></label><input type="password" id="p2" name="confirmPassword" /><br/><font color="red"><div id ="lp3" ></div></font>

					<label for="uID">E-mail :<font color="red">*  </font></label><input id="uid" type="email" name="uID" /><br/>
					<label for="Country">Country :<font color="red">*  </font></label><input id="Country" type="text" name="Country" /><br/>
					<label for="uID">Affiliation :<font color="red">*  </font></label><input id="Affiliation" type="text" name="Affiliation" /><br/>
					<INPUT type=submit name="Register" value=Register >
					
				</FORM>
				</div>
				</div>
				</center>
				
			
		
		</center>
				<?php
				
				
			}
			
		  }
		
	break;
	// Add a new device
	case "device_add":
	?>
	
	
	<div id="addDev">
	             <center>
			
				 <header>
			<h3>Device registration Form</h3></header>
	             <p>
					Please complete the form below.Mondatory fields marked <font color="red">*</font>
				</p>
	<div id="lp0"></div></center>
	
	
		<FORM id="addDev" Method="POST" Action="fichier.php?homeagent=<?php echo $_SESSION['homeagent'] ?>&username=<?php echo $user->getUsername() ?>">
			<label>Type of your device :<font color="red">*  </font></label>
			<SELECT name="device_type" id="device_type" >
				<OPTION>Public computer</OPTION>
				<OPTION>Personal computer</OPTION>
				<OPTION>Personal laptop</OPTION>
				<OPTION>Mobile Phone</OPTION>
				<OPTION>Tablet</OPTION>
			<SELECT><br/>
			
			<label>Name of your device :<font color="red">*  </font></label><input type="text" name="device_name"  id="device_name"/><br />
								<font color="green"><div id="suggest"></div></font>

			<label>Device Port :<font color="red">*  </font> </label><input type="text" name="device_port" id="device_port"/> <br/>
			<label>Public folder:<font color="red">*  </font></label> <input type="text" name="public_folder" id="public_folder"/> <br/>
			<label>Private Folder:<font color="red">*  </font> </label> <input type="text" name="private_folder" id="private_folder"/> <br/>
			<label>OS:<font color="red">*  </font> </label> <input type="text" name="os" id="os"/> <br/>
			<label>Description: <font color="red">*  </font></label> <TEXTAREA rows="3" name="description" id="description" ></TEXTAREA><br/>
			<label>Publicly indexed: <font color="red">*  </font></label> <input type="radio" name="index" value="yes" id="index0"> Yes           <input type="radio" name="index" value="no" id="index1" checked >No <br/>
			<center><font color="green">(if you choose "yes",your public folder will be indexed by the crawlers for public access.)</font></center>
			<center><INPUT type=submit name="add_device" value="Add a new device">			<INPUT type=button name="cancel" value="cancel" onclick="cancelMod();">
</center>
		</FORM>
		
		</div>
	<?php
	break;
	
		
//----------------------------------------------------INDEX-----------------------------------------------------------------
		
	case "index":
		if(!isset($_SESSION['connect'])){
			
			echo "please log in ";
			
		}else{
				
	
			
			?>
			<b>Welcome to pWeb <?php echo $user->getUsername();?></b>
			
			<?php
			
    	}
	break;
		


}


if(isset($user)){ $_SESSION['user'] = $user; }

?>
  		
	
		
	
	</section>
	

	
		
	<footer>
    <section>
    <h3>about pweb</h3>
   
    
    </section>
    
    <section>
    <h3>Help</h3>
    <p>FAQs</p>
   
    </section>
    
    <section>
    <h3>about us</h3>
    <p>&copy; 2013 <a href="#" title="your site name">pWebsite.com</a> All rights reserved.</p>
    </section>

	</footer>

</body>
</html>
