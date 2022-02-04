const TodoButton = {
   template: `
      <button><slot>DefaultText</slot></button>
   `
}

const RootComponent = {
   components: {
      'todo-button': TodoButton
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
