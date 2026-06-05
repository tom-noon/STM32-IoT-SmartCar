<template>
	<view style="width:50%;">
		<uni-card thumbnail="../../static/images/rentCar.png" :title="title" :num="num" style="flex:none;margin: 5px;" :class="hour>12?'active':''">
			<h1 style="margin: 20px;">{{num}}</h1>
		</uni-card>
	</view>
</template>

<script>
	export default {
		name:"workState",
		props:{
			title:{
				type:String,
			},
			num:{
				
			}
		},
		data() {
			return {
				hour:'00',
				min:'00',
				miao:'00',				
				timer:null
			};
		},
		methods:{
		},
		mounted() {

		},
		beforeDestroy() {
		}
	}
</script>

<style>
.active{
	background-color: red;
}
.active view{
	color: white;
}
</style>