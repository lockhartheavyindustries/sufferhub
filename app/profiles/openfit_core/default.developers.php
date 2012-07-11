<?php
global $base_url;
$api_base = $base_url . '/openfitapi/api/';
?>
<p>Want to integrate your mobile app or website with us? No problem!</p>
<p>Our website allows access to workout information via the <strong><a href='http://www.openfitapi.com/'>Open Fitness API</a></strong></p>
<p>For complete documentation, samples, or to get involved in the Open Fitness API project, visit <a href='http://www.openfitapi.com/'>http://www.openfitapi.com/</a></p>
<h2>Public activity feed</h2>
<p>To get a list of public workouts, send an HTTP GET request to this URL:</p>
<p><strong><?php echo $api_base . 'fitnessActivities.json'; ?></strong></p>
<p>You may use the following parameters to determine which workouts are returned:</p>
<ul>
<li><strong>pageSize</strong>: Number of results to return. Defaults to 25.</li>
<li><strong>page</strong>: Which page of results to return. Defaults to 0.</li>
<li><strong>noEarlierThan</strong>: If specified limits the date range of results returned.</li>
<li><strong>noLaterThan</strong>: If specified limits the date range of results returned.</li>
</ul>
 <p>Each result includes summary information about a workout and a URI to access detail data.</p>
<h2>Accessing user data</h2>
<p>To access private user data you'll first need to authenticate and get a session cookie.</p>
<p>To authenticate send an HTTP POST request to this URL:</p>
<p><strong><?php echo $api_base . 'user/login'; ?></strong></p>
<p>Include the following parameters as a JSON encoded object:</p>
<ul>
<li><strong>username</strong>: The username.</li>
<li><strong>password</strong>: The password.</li>
</ul>
<p>You'll also need to set the following HTTP headers</p>
<ul>
<li><strong>Accept</strong>: application/json</li>
<li><strong>Content-Type</strong>: application/json</li>
<li><strong>Content-length</strong>: Length of the JSON data.</li>
</ul>
<p>If successful the response will be a JSON string that contains two elements: <strong>session_name</strong> and <strong>sessid</strong>.</p>
<p>To get a list of workouts for the authenticated user send an HTTP GET request to this URL:</p>
<p><strong><?php echo $api_base . 'fitnessActivities.json'; ?></strong></p>
<p>And set a request cookie for:</p>
<p><em>session_name value</em> = <em>sessid value</em></p>
<p>The response and optional parameters are identical to the public workout feed.</p>
<p>To request workout details, send HTTP GET requests to the URI in the workout response and include the session cookie.</p>
<h2>Creating a new workout</h2>
<p>An authenticated user can create a new workout by sending an HTTP POST to this URL:</p>
<p><strong><?php echo $api_base . 'fitnessActivities'; ?></strong></p>
<p>Include the workout data as a JSON encoded object in the POST parameters and be sure to include the session cookie you received from authentication.</p>
<p>Everything except the activity <em>start_date</em> parameter is optional and reasonble defaults will be used.</p>
<h2>Updating and deleting a workouts</h2>
<p>At this moment <strong>update</strong> and <strong>delete</strong> operations are not supported.</p>
<h2>Additional information</h2>
<p>For complete API documentation and code samples visit <a href='http://www.openfitapi.com/'>http://www.openfitapi.com/</a></p>