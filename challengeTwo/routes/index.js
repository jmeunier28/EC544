var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'EC544 Challenge two', name: 'Group 04' });
});

module.exports = router;
