	<?php
		$nodefp = fopen("nodes", "r");
					$n = fgets($nodefp, 1024);
					$list=array();
					for($i = 0; $i < $n; $i++)
					{
						$line = fgets($nodefp, 1024);
						$ha_name = explode(" ", $line);
						$list[$i]= $ha_name[3];
						
					}

					fclose($nodefp); 
					
					$array['ha']=$list;
echo json_encode($array);
					
				?>