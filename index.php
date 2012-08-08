<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">

<head>

	<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1"/>
    <meta charset="utf-8"/>
    <meta name="description" content="">
    <meta name="author" content="">
	<!-- Facebook OpenGraph -->
	<meta property="og:title" content="SufferHub" />
	<meta property="og:type" content="company" />
	<meta property="og:url" content="http://sufferhub.com" />
	<meta property="og:image" content="http://sufferhub.com/images/sufferhub_fb_logo.png" />
	<meta property="og:site_name" content="SufferHub" />
	<meta property="fb:admins" content="759603155" />
	<!-- end Facebook OpenGraph -->


<title>SufferHub</title>

	<link rel="stylesheet" href="css/style.css" type="text/css" media="screen" />
	<link rel="stylesheet" href="css/nivo-slider.css" type="text/css" />
	<link rel="stylesheet" href="css/jquery.fancybox-1.3.4.css" type="text/css" />


	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.6/jquery.min.js"></script>
	<script>window.jQuery || document.write('<script src="js/jquery-1.6.1.min.js"><\/script>')</script>

	<script src="js/jquery.smoothscroll.js"></script>
	<script src="js/jquery.nivo.slider.pack.js"></script>
	<script src="js/jquery.easing-1.3.pack.js"></script>
	<script src="js/jquery.fancybox-1.3.4.pack.js"></script>
	<script src="js/init.js"></script>

	<!-- Google Analytics tracking code -->
	<script type="text/javascript">
	  var _gaq = _gaq || [];
	  _gaq.push(['_setAccount', 'UA-19242775-8']);
	  _gaq.push(['_trackPageview']);
	  (function() {
	    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
	    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
	    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
	  })();
	</script>
	<!-- end Google Analytics tracking -->

</head>

<body>
	<!-- Facebook -->	
	<div id="fb-root"></div>
	<script>(function(d, s, id) {
	  var js, fjs = d.getElementsByTagName(s)[0];
	  if (d.getElementById(id)) return;
	  js = d.createElement(s); js.id = id;
	  js.src = "//connect.facebook.net/en_US/all.js#xfbml=1&appId=418202894893170";
	  fjs.parentNode.insertBefore(js, fjs);
	}(document, 'script', 'facebook-jssdk'));</script>
	<!-- end Facebook -->	


<div id="wrapper">
	<?php include('includes/variables.php'); ?>
	<?php include('includes/header.php'); ?>
	
	<div id="content" class="content-wrap">
		    <section id="home">
		        <div class="intro-box">
		           <h1>The SufferHub Project</h1>
		           <p class="intro">We're here to help you hit your fitness goals by making exercise more fun, more social, and more rewarding.  We help you use your workout data to have fun, stay motivated, earn prizes, and gain new insights by using that data in new ways.  We believe exercise should be about more than just suffering!</p>

				<!-- sports icons -->
				<p><img alt="" src="app/sites/all/modules/openfit_api/images/category-cycling.png"/>&nbsp&nbsp<img alt="" src="app/sites/all/modules/openfit_api/images/category-running.png"/>&nbsp&nbsp<img alt="" src="app/sites/all/modules/openfit_api/images/category-hiking.png"/>&nbsp&nbsp<img alt="" src="app/sites/all/modules/openfit_api/images/category-walking.png"/>

		        </div>

		        <div class="slider-wrapper">

		            <div id="slider" class="nivoSlider">
		                <img src="images/slides/slide1.png" width="383" height="198" alt="" />
		                <img src="images/slides/slide2.png" width="383" height="198" alt="" />
		                <img src="images/slides/slide3.png" width="383" height="198" alt="" />
		            </div>
		        </div>
		        <div class="row no-bottom-margin">
					<H1>How SufferHub Works:</H1>
		            <section class="col">
		                <h2>Suffer</h2>

		                <p>Do your workout of choice. We currently support the following GPS motion-tracked activities: cycling, running, hiking, and walking.  Capture your data automatically using our <a href="#mobile">mobile app</a>, or use the .FIT files you're already capturing from your <a href="http://www.garmin.com/us/products/intosports">Garmin fitness device!</a></p>
		            </section>
		            <section class="col mid">
		                <h2>Share</h2>

		                <p><a href="/app">Upload your workout file</a>, share your progress with your fitness-minded friends and socialize with fellow SufferHub members.  Participate in online competitions with friends and fellow SufferHub members worldwide!</p>
		            </section>

		            <section class="col">
		                <h2>Score!</h2>

		                <p>Your workout data is worth more at SufferHub! You earn game achievements, leaderboard placement, and <a href="#sufferbucks">SufferBucks</a> points with every workout, which power our online competitions AND earn you discounts on fitness equipment from our <a href="#partners">affiliate partners</a>!</p>
		            </section>
		        </div>

			<h3>Let's get started!</h3>
			<p class="intro">
				<ol>	
					<li><strong><a href="/app">Join our Beta</a></strong> and start uploading your workouts!<br><a href="/app"><img src="images/signup_button.png" width="150" height="50"></a></li>
					<li><strong><a href="#about-us">Read more</a></strong> about us and our vision</li>
					<li><strong>Like us</strong> on <a href="https://www.facebook.com/SufferHub" target = "_blank">Facebook</a> 
					<div class="fb-like" ref="homepage_top" data-href="http://sufferhub.com" data-send="false" data-width="450" data-show-faces="false"></div>
					</li>
					<li><strong>Follow us</strong> on <a href="https://twitter.com/SufferHub" target = "_blank">Twitter</a> 
					<a href="https://twitter.com/SufferHub" class="twitter-follow-button" data-show-count="false">Follow @SufferHub</a>
					<script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src="//platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script>
					</li>
				</ol>

		        <a class="back-to-top" href="#top">Back to Top</a>

		      </section>


		      <section id="applications" >

		             <h1>Applications</h1>
		             <div class="row no-bottom-margin">
		                <section class="col">
							<h2><a href = "/app">SufferHub Web</a></h2>
		                    <p><img class="align-left" alt="" src="images/services/webdesign.png" /><a href="/app">Our core online service</a>.  Upload your workouts, analyze your performance, share with friends, and join in virtual competitions.  All while earning <a href="#sufferbucks">SufferBucks</a> points!  You get more value out of your suffering at SufferHub.</p>                    
		                </section>
		                <section class="col mid">
							<h2><a href="#multiloader">SufferHub Multi-Loader</a></h2>
		                    <p><img class="align-left" alt="" src="images/services/webdevelopment.png" />If you're like us, you upload your workout files to more 		than one service.  Let us help out!  Use our <a href="#multiloader">online multi-loader</a> to upload your workouts to sites like Strava, Garmin Connect, and (of course) <a href="/app">SufferHub</a> - all in one click!</p>
		                </section>
		                <section class="col">
		                    <h2><a href="#mobile">SufferHub Mobile</a></h2>

		                    <p><img class="align-left" alt="" src="images/services/logo-design-and-branding.png" />No Garmin device?  No problem!  Get the <a href="#mobile">SufferHub mobile app</a> and log your workouts automatically with your iPhone, then upload them to <a href="/app">SufferHub</a> when you're finished!</p>

		                </section>
		            </div>

		            <a class="back-to-top" href="#top">Back to Top</a>

		      </section>


		      <section id="games">

		            <h1>Games</h1>

		            <div class="row no-bottom-margin">
		                <section class="col">
		                    <h2><a href="#sufferchallenge">Put Up or...</a></h2>
		                    <p><img class="align-left" alt="" src="images/services/logo-design-and-branding.png" /> Studies show you're more likely to follow through on a commitment if you publicly sign up to do it.  This game lets you set a challenge for yourself that your friends see also, then rewards you in <a href="#sufferbucks">SufferBucks</a> when you complete (or exceed) it!</p>
		                </section>
		                <section class="col mid">
		                    <h2><a href="#sufferscratchrace">Handicap Races</a></h2>

		                    <p><img class="align-left" alt="" src="images/services/logo-design-and-branding.png" /> Sure, that 110lb, 20 year old college kid can beat you in a real world race, but how would she do against you if things were adjusted for each competitor's % of maximum ability?  We'll set up fun virtual competitions that measure progress based on more variables than just time...</p>
		                </section>

		                <section class="col">
		                    <h2><a href="#sufferindoor">Indoor Races</a></h2>

		                    <p><img class="align-left" alt="" src="images/services/logo-design-and-branding.png" />Even when your workouts are indoors on a trainer, treadmill, or stairmaster you can still play at <a href="/app">SufferHub</a>! Compete head to head against your friends over the internet, or download your friend's best workout as a virtual rabbit to chase!</p>

		                </section>
		            </div>
		            <a class="back-to-top" href="#top">Back to Top</a>

		      </section>

		     <section id="insights" >

		             <h1>Insights</h1>

		             <div class="row no-bottom-margin">
		                <section class="col">
		                    <h2><a href="#traffictracker">Traffic Analysis</a></h2>
		                    <p><img class="align-left" alt="" src="images/services/webdesign.png" />What if your ride or run data could be used by city planners in your hometown to make better decisions about creating bike lanes, making sidewalk repairs, etc?  By working with local and regional advocacy groups we can share SufferHub's anonymous usage data of roads and trails with government agencies and make your fitness data work on your behalf to make your hometown a better place to exercise!</p>
		                </section>
		                <section class="col mid">
		                    <h2><a href="#suffermatch">Workout Partner Finder</a></h2>

		                    <p><img class="align-left" alt="" src="images/services/webdesign.png" />Odds are, you typically use a handful of routes on a regular basis, at similar times of day.  What if we could use that data to suggest potential workout partners based on where you both go, and how fast you travel?  You might be able to find your next riding, running, hiking, or walking buddy!</p>
		                </section>

		                <section class="col">
		                    <h2><a href="#suffergrid">Geotagged Data Mashups</a></h2>

		                    <p><img class="align-left" alt="" src="images/services/webdesign.png" />Your GPS-tagged workout data can be overlaid against other data sets with similar GPS and timestamp data.  Imagine being able to search for a route by location, then being presented with a selection of photos and videos taken at that location!  And what if you could upload and share your own photos and video along with your workout data to share with others?</p>

		                </section>
		            </div>

		            <a class="back-to-top" href="#top">Back to Top</a>

		      </section>

		      <section id="about-us" class="clearfix">

		            <h1>About The SufferHub Project</h1>

		            <div class="primary">

		                <p class="intro">SufferHub occupies the convergence zone of fitness, online gaming, and big data.  Our primary motivation is to encourage people to be more healthy and active, by rewarding healthy behavior.  By making workouts into games with rewards, we hope to help people set and maintain a healthy and active lifestyle.  But SufferHub was also inspired by the notion that we're not getting as much value as we could be out of the massive amount of location + fitness data being captured by individuals all over the world.  We've got increasingly widespread adoption of smartphones and workout-specific measurement devices that allow us to gather GPS coordinates, speed, distance, heartrate, and even power output.  Currently, most of that fitness data lives locked away in private databases with no ability for others to access it for wider trend analysis or correlation with other datasets.  We believe we can do more than simply examine our own data in isolation, or in direct comparison to other "pockets" of users.  We believe that this data should be gathered, anonymized (scrubbed of all personal information), then shared with others in order to enable new insights.  It's time to start having more fun while working out, and to start finding new ways to make this data benefit the people who created it in the first place!
		                </p>

<!--
		                <div class="row no-bottom-margin">

		                    <section class="col first">

		                        <h2>Our Vision</h2>

		                        <p>We live in a "Big Data" era where seemingly every event is tracked and stored somewhere.  Measuring and storing is the easy part; the hard part is making sense of the data, especially when combining it with potentially related data from other sources.  We at SufferHub are passionate about fitness, technology, and games - and we want to combine these passions to help others in the fitness community in ways that weren't possible until now.</p>

		                    </section>

		                    <section class="col">

		                        <h2>Our Approach</h2>

		                        <p>This is a big task, so we're going to form an army!  We'll gather fitness data from our community of users via the SufferHub online service.  By providing compelling and entertaining new ways to play games using workout data, we hope to attract people to our cause.  As we identify more and more potential <a href ="#insights">insights</a> to go after, we'll reach out and partner with other organizations in the interest of sharing our fitness data on a larger scale through an open, published data API.  And we'll publish our findings here on SufferHub for everyone!</p>

		                    </section>

		                </div>
-->
		                <h2>Our Team</h2>

		                <ul class="the-team">
		                    <li class="odd">
		                        <div class="thumbnail">
		                            <a href="/app/users/brianlockhart"><img alt="thumbnail" src="images/brian_headshot.jpg" width="83" height="78"></a>
		                        </div>
		                        <p class="mname"><a href="/app/users/brianlockhart">Brian Lockhart</a></p>
		                        <p>Founder, CSO</p>
								<p>(Chief SufferHub Officer)</p>

		                    </li>
		                    <li>
		                        <div class="thumbnail">
		                            <a href="/app/users/stephenpearce"><img alt="thumbnail" src="images/stephen_headshot.jpg" width="83" height="78"></a>
		                        </div>
		                        <p class="mname"><a href="/app/users/stephenpearce">Stephen Pearce</a></p>
		                        <p>Data Scientist in Residence</p>
		                    </li>
		                    <li class="odd">
		                        <div class="thumbnail">
		                            <a href="#"><img alt="thumbnail" src="images/thumb-pic.png" width="83" height="78"></a>
		                        </div>
		                        <p class="mname"><a href="#">TBA</a></p>
		                        <p>Director of Game Design</p>
		                    </li>
		                    <li>
		                        <div class="thumbnail">
		                            <a href="#"><img alt="thumbnail" src="images/thumb-pic.png" width="83" height="78"></a>
		                        </div>
		                        <p class="mname"><a href="#">TBA</a></p>
		                        <p>Director of Development</p>
		                    </li>
		                </ul>

		            </div>

		            <aside>
		<!--		
						<h2>Funding</h2>
						We're currently bootstrapping our operations and running lean, but as we bring on more users and start to tackle our more ambitious feature development tasks, costs will ramp up.  Interested investors please contact us at <a href="mailto:info@sufferhub.com">info@sufferhub.com</a>.
						</p>
						For now, if you'd like to donate to the project directly, we'd greatly appreciate your contribution.  Clouds in the sky are free, but cloud services run on dollars!
						</p></p>
						<form action="https://www.paypal.com/cgi-bin/webscr" method="post">
						<input type="hidden" name="cmd" value="_s-xclick">
						<input type="hidden" name="hosted_button_id" value="H6WJT45LTTKZQ">
						<p align="center"><input type="image" src="https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
						<img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif" align="center" width="1" height="1">
						</p>
						</form>

						</p>
					-->	
					<!-- Begin Official PayPal Seal
					<a href="https://www.paypal.com/us/verified/pal=info%40lockhartheavyindustries%2ecom" target="_blank"><img src="https://www.paypal.com/en_US/i/icon/verification_seal.gif" border="0" alt="Official PayPal Seal"></A>
					End Official PayPal Seal -->

		<!--
		                    <a href="#" class="download-btn">Download PDF</a>


		                    <h2>Partner Links</h2>

		                    <ul class="link-list">
								<li><a href="http://sufferhub.com" title="SufferHub">SufferHub</a></li>
								<li><a href="http://sufferhub.com" title="SufferHub">SufferHub</a></li>
								<li><a href="http://sufferhub.com" title="SufferHub">SufferHub</a></li>
								<li><a href="http://sufferhub.com" title="SufferHub">SufferHub</a></li>
							</ul>


		                    <h2>Testimonials</h2>

		                    <div class="testimonials">
		                        <blockquote>
		                            <p>Donec sed odio dui. Nulla vitae elit libero, a pharetra augue.
		                            Nullam id dolor id nibh ultricies vehicula ut id elit. </p>

		                            <cite>&mdash; John Doe, XYZ Company</cite>
		                        </blockquote>
		                        <blockquote>
		                            <p>Aenean lacinia bibendum nulla sed consectetur. Cras mattis
		                            consectetur purus sit amet fermentum.</p>

		                            <cite>&mdash; Jane Roe, ABC Corp</cite>
		                        </blockquote>
		                    </div>
		-->
		            </aside>



		            <a class="back-to-top" href="#top">Back to Top</a>

		      </section>



		      <section id="contact-us" class="clearfix">

		            <h1>Contact the SufferHub team</h1>

		            <div class="primary">

		                <p class="intro">
		                We'd love to have you join the SufferHub community, and hear your ideas for how we can make this a more fun and rewarding place for our members.  Check out the resources listed on the right, and choose your favorite methods of getting in touch with our team.  
		                </p>

<img alt="" src="images/SufferHub_Logo_square.png" width="560" height="400">
						<!-- Begin MailChimp Signup Form -->
<!--
						<div id="mc_embed_signup">
						<form action="http://sufferhub.us5.list-manage1.com/subscribe/post?u=937bd3c7ddf2827546233f1ec&amp;id=bf8374dc23" method="post" id="mc-embedded-subscribe-form" name="mc-embedded-subscribe-form" class="validate" target="_blank">
							<h2>Subscribe to the SufferHub mailing list</h2>
						<div class="indicates-required"><span class="asterisk">*</span> indicates required</div>
						<div class="mc-field-group">
							<label for="mce-EMAIL">Email Address  <span class="asterisk">*</span>
						</label>
							<input type="email" value="" name="EMAIL" class="required email" id="mce-EMAIL">
						</div>
						<div class="mc-field-group">
							<label for="mce-FNAME">First Name  <span class="asterisk">*</span>
						</label>
							<input type="text" value="" name="FNAME" class="required" id="mce-FNAME">
						</div>
						<div class="mc-field-group">
							<label for="mce-LNAME">Last Name  <span class="asterisk">*</span>
						</label>
							<input type="text" value="" name="LNAME" class="required" id="mce-LNAME">
						</div>
							<div id="mce-responses" class="clear">
								<div class="response" id="mce-error-response" style="display:none"></div>
								<div class="response" id="mce-success-response" style="display:none"></div>
							</div>	<div class="clear"><input type="submit" value="Add me!" name="subscribe" id="mc-embedded-subscribe" class="button"></div>
						</form>
						</div>
-->
						<!--End mc_embed_signup-->

		            </div>

		            <aside>
							<h2>Join our community!</h2>

							<li>Check out the <strong><a href="/blog">SufferHub blog</a></strong></li>
							<li>Join the <strong><a href="/app/forum">SufferHub forums</a></strong></li>
							</p>
							<h2>Follow SufferHub!</h2>

		                    <ul class="link-list social">
		                        <li class="facebook"><a href="https://www.facebook.com/SufferHub">Facebook</a><div class="fb-like" ref="homepage_bottom" data-href="http://sufferhub.com" data-send="false" data-width="450" data-show-faces="false"></div></li>
								<li class="twitter"><a href="https://twitter.com/SufferHub" target="_blank">Twitter</a><a href="https://twitter.com/SufferHub" class="twitter-follow-button" data-show-count="false">Follow @SufferHub</a>
								<script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0];if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src="//platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script></li> 
								<!--                       
								<li class="googleplus"><a href="#">Google+</a>(coming soon)</li>
		                        <li class="linkedin"><a href="#">Linkedin</a>(coming soon)</li>
								-->
		                    </ul>

		                    <h2>Contact</h2>                    
							<a href="mailto:info@sufferhub.com">info@sufferhub.com</a>
		                    </p>
		                   	
							11410 NE 124th St. #512<br>
							Kirkland, WA 98034
							</p>
							800.750.0987
		            </aside>

		            <a class="back-to-top" href="#top">Back to Top</a>

		     </section>

		</div>

	</div> <!-- end #content -->


<?php include('includes/footer.php'); ?>

</div> <!-- End #wrapper -->

</body>

</html>
