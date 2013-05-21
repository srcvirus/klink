
	
	$(document).ready(function() {
    /////login///////
	
	//$('#authform').hide();
	$('#selectDev').hide();
	
	$('#ok').click( function(){ 
	    $('#selectHA').hide();
        $('#authform').show(); 
             });
			 
	
			 
			 
    $('#authentificationForm').on('submit', function() {
 
       
         var Username = $('#Username').val();
         var Password = $('#Password').val();
		 var ha =  '<?php if (isset ($_SESSION['homeagent'])) echo $_SESSION['homeagent']; ?>';
        

     
        if(Username == '' || Password == '') {
		    $('#auth').html('<b>Filling all the inputs is required </b>');
            //alert('Les champs doivent êtres remplis');
        } else {
            // appel Ajax
            $.ajax({
                url: 'fichier.php?homeagent='+ha+'&username='+Username+'&password='+Password, //ha+'/?method=authenticate &username='+Username+'&password='+Password
                type: $(this).attr('method'), 
                data: $(this).serialize(), 
                dataType: 'json',
                success: function(json) {
                    if (json.status == 'success')
					{
					    
					
					
					$.ajax({
                url: 'pWebcs.php?fn=check_login&homeagent='+ha+'&username='+Username+'&password='+Password, 
                type: 'POST', 
                data: $(this).serialize(), 
           
										 
										 success: function()
										 {
										     // $('#regMsg').html('Registration completed successfully ');
										 	  window.location.href='pWebcs.php?fn=index';
										 }
				
								
								
								/*
								url: 'fichier.php?homeagent='+ha+'&username='+Username+'&password='+Password, //homeagent/?method=getDeviceList&username='+Username,
								type: 'POST', 
								data: $(this).serialize(), 
								dataType: 'json',
								success: function(json) {
								$.each(json.devices, function(i) {
								$('#dev').append('<option value="'+ json.devices[i].name +'">'+ json.devices[i].name +'</option>');
								});
								
									$('#authform').hide();
									$('#selectDev').show();
					
					*/
					
					
								
					
            });
					
					}
                        
						else
						 $('#auth').html('<b>Wrong username or password</b>');
                    
                }
            });
        }
        return false; // j'empêche le navigateur de soumettre lui-même le formulaire
    });
	
		$('#registerForm').on('submit', function() 
	{
	
 
		
		var u = $('#uname').val();
        
		var p1= $('#p1').val();
		var p2= $('#p2').val();
		var uid= $('#uid').val();
		
		
		var text='<font color="red"><b>This account already exists, please change your username!</b></font></br> you can choose from this list: </br> (';
		var url2=$(this).attr('action');
		            if ( u =='' || p1 =='' || p2 =='' || uid =='' )
					{
						$('#lp0').html(' <b>Filling all the compulsary inputs is required </b>');
					}
					else if (p1 != p2)
					{
					    $('#lp3').html( '<b>Password does not match confirmation</b>');
				
					}
					else
					{
					
					
					//check availability
					$.ajax({
					
					//$(this).attr('action')   must contain only home agent
                url: $(this).attr('action')+'&username='+ u +'&password='+ p1,  // $(this).attr('action')+'/?method=existUsername&name='+u,
                type: 'POST', 
                data: $(this).serialize(), 
                dataType: 'json',
			    success: function(json)
				{
					if (json.availability == 'yes')
				    { //registration
					
						$.ajax({
					
						//$(this).attr('action')   must contain only home agent
						url: url2+'&username='+ u +'&password='+ p1,  // $(this).attr('action')+'/?method=registerUser&name='+u+'&password='+p1+'&email='+uid
						type: 'POST', 
						data: $(this).serialize(), 
						dataType: 'json',
						success: function(json)
						{
							  if (json.status == 'success')
							  {
							  
									$.ajax({
					
                                         url: 'pWebcs.php?fn=connect&username='+u+'&password='+p1,   
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
				}
								
									
					
					
						});
                    
					    
								
					
                }
            
	return false; 
	});
	
	
	

	
	 /*
	$('#selectDevice').on('submit', function() {
	
	alert( $('#dev').val());
	});
	*/
});


