<?php


//$username=$_GET['username'];





//$reponse = 'not ok';

$array['name']='asd';
$array['availability'] = 'yes';
$array['status'] = 'success';
//$array['reponse']=$password;
//$array['devices']=array(array('name' => 'mylaptop', 'type' => 'laptop','port' => '9999', 'public_folder' => 'C://public' , 'private_folder' => 'C://private' ),array('name' => 'mylaptop2', 'type' => 'laptop2','port' => '8888', 'public_folder' => 'C://public2' , 'private_folder' => 'C://private2'));
$array['suggestion']=array('name1','name2','name3','name4');
echo json_encode($array);

?>
